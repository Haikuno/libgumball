#include <gumball/core/gumball_manager.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/types/gumball_texture.h>
#include <gumball/types/gumball_font.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <unistd.h>

// TODO: make these private variables!
static GblHashSet GUM_Manager_hashSet_;
static char       GUM_Manager_currentPath_[1024];

typedef struct GUM_HashSetEntry {
    GUM_IResource* pResource;
    GblQuark       quark;
} GUM_HashSetEntry;

static GblHash resourceHasher_(const GblHashSet* pSet, const void* pItem) {
    GBL_UNUSED(pSet);
    GUM_HashSetEntry* pEntry = (GUM_HashSetEntry*)pItem;
    return gblHash(&pEntry->quark, sizeof(GblQuark));
}

static GblBool resourceComparator_(const GblHashSet* pSelf, const void* pEntry1, const void* pEntry2) {
    GBL_UNUSED(pSelf);
    const GUM_HashSetEntry* pResEntry1 = (const GUM_HashSetEntry*)pEntry1;
    const GUM_HashSetEntry* pResEntry2 = (const GUM_HashSetEntry*)pEntry2;
    return pResEntry1->quark == pResEntry2->quark;
}

static bool isExtension(const GblStringView path, GblStringRef** extensions, GblStringRef** outExt) {
    for (size_t i = 0; extensions[i]; i++) {
        if (GblStringView_endsWith(path, extensions[i])) {
            *outExt = extensions[i];
            return true;
        }
    }

    return false;
}

GBL_EXPORT GUM_IResource* GUM_Manager_load(GblStringRef* path) {
    GUM_LOG_DEBUG_PUSH("GUM_Manager_load() called...");

    if (!path) {
        GUM_LOG_ERROR("Path passed to GUM_Manager_load() is null");
        return nullptr;
    }

    GUM_LOG_DEBUG("Path is %s", path);

    GblClass*       managerClass = GblClass_refDefault(GUM_MANAGER_TYPE);
    GblStringBuffer stringBuffer;

    GUM_LOG_DEBUG_SCOPE("Creating string buffer...") {
        if (!GblStringBuffer_construct(&stringBuffer, path)) {
            GUM_LOG_ERROR("Failed to construct string buffer!");
        }
        GUM_LOG_DEBUG("String buffer created successfuly!");
    }

    GblStringBuffer_prepend(&stringBuffer, "/");
    GblStringBuffer_prepend(&stringBuffer, GUM_Manager_currentPath_);

    GblStringRef* fullPath = GblStringBuffer_cString(&stringBuffer);
    GUM_LOG_DEBUG("Full path is %s", fullPath);

    GUM_HashSetEntry entry = { .pResource = nullptr, .quark = GblQuark_fromString(fullPath) };

    // Check if the resource is already loaded
    if (GblHashSet_contains(&GUM_Manager_hashSet_, (const void*)&entry)) {
        GUM_LOG_DEBUG("Resource was already loaded! Returning a ref");
        entry = *(GUM_HashSetEntry*)GblHashSet_at(&GUM_Manager_hashSet_, &entry);
        goto end;
    }

    GUM_LOG_DEBUG_SCOPE("Resource was not loaded before! Loading...") {
        GUM_LOG_DEBUG_PUSH("Checking if file exists");

        FILE* file = fopen(fullPath, "rb");

        if (!file) {
            GUM_LOG_ERROR("File does not exist!");
            GBL_LOG_POP(1);
            GBL_SCOPE_EXIT;
        }

        GUM_LOG_DEBUG("File exists!");
        GBL_LOG_POP(1);
        fclose(file);

        GblStringView stringView = GblStringView_fromString(fullPath);
        GblStringRef* extension  = nullptr;

        // TODO: supported extensions should be backend specific

        static GblStringRef* texture_extensions[] = { ".png", ".bmp", ".tga",  ".jpg", ".gif", ".hdr",  ".pic", ".psd",
                                                      ".dds", ".ktx", ".ktx2", ".pkm", ".pvr", ".astc", nullptr };

        static GblStringRef* font_extensions[] = { ".ttf", ".otf", ".fnt", ".bdf", nullptr };

        GblType resourceType = 0;
        GUM_LOG_DEBUG_SCOPE("Checking for resource type...") {

            if (isExtension(stringView, texture_extensions, &extension)) {
                GUM_LOG_DEBUG("Resource is a texture! loading...");
                resourceType = GUM_TEXTURE_TYPE;
                GBL_SCOPE_EXIT;
            }

            if (isExtension(stringView, font_extensions, &extension)) {
                GUM_LOG_DEBUG("Resource is a font! loading...");
                resourceType = GUM_FONT_TYPE;
                GBL_SCOPE_EXIT;
            }
        }

        if (!resourceType) {
            GUM_LOG_ERROR("File extension not supported yet!");
            GBL_SCOPE_EXIT;
        }

        entry.pResource = GUM_IRESOURCE(GblBox_create(resourceType));
        GUM_IRESOURCE_CLASSOF(entry.pResource)->pFnLoad(entry.pResource, fullPath);
        GUM_IRESOURCE_CLASSOF(entry.pResource)->pFnSetQuark(entry.pResource, entry.quark);

        GblHashSet_insert(&GUM_Manager_hashSet_, &entry);

        GUM_LOG_DEBUG("Resource loaded successfuly!");
    }

end:
    GUM_LOG_POP(1);
    GblClass_unrefDefault(managerClass);
    GblStringBuffer_destruct(&stringBuffer);

    if (entry.pResource) return GUM_IResource_ref(entry.pResource);
    return nullptr;
}

GBL_EXPORT void GUM_Manager_unload(GUM_IResource* pResource) {
    GUM_LOG_DEBUG_SCOPE("GUM_Manager_unload() called...") {
        if (!pResource) {
            GUM_LOG_ERROR("Resource passed to GUM_Manager_unload() is null");
            GBL_SCOPE_EXIT;
        }

        GblQuark quark;
        GUM_IRESOURCE_CLASSOF(pResource)->pFnQuark(pResource, &quark);

        GUM_HashSetEntry entry = { .pResource = pResource, .quark = quark };

        GUM_LOG_DEBUG_SCOPE("Checking if the resource is loaded...") {
            if (!GblHashSet_contains(&GUM_Manager_hashSet_, (const void*)&entry)) {
                GUM_LOG_ERROR("Attempted to unload a resource that was not loaded before!");
                GBL_SCOPE_EXIT;
            }

            GUM_LOG_DEBUG_SCOPE("The resource is currently loaded! Unreffing...") {
                GblHashSet_erase(&GUM_Manager_hashSet_, (const void*)&entry);
                GUM_IResource_unref(pResource);

                GUM_LOG_DEBUG_SCOPE("Checking if the resource is still being used...") {
                    if (GblBox_refCount(GBL_BOX(pResource)) > 1) {
                        GUM_LOG_ERROR("Attempted to unload a resource that is still being used!");
                        GBL_SCOPE_EXIT;
                    }

                    GUM_LOG_DEBUG("No references left! Unloading...");
                    GUM_IRESOURCE_CLASSOF(pResource)->pFnUnload(GUM_IRESOURCE(pResource));
                }
            }
        }
    }
}

GBL_RESULT GUM_ManagerClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_MANAGER_TYPE)) {
        GblHashSet_construct(&GUM_Manager_hashSet_, sizeof(GUM_HashSetEntry), resourceHasher_, resourceComparator_);
        getcwd(GUM_Manager_currentPath_, 1024);
    }

    return GBL_RESULT_SUCCESS;
}

GblType GUM_Manager_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Manager"),
                                GBL_STATIC_CLASS_TYPE,
                                &(static GblTypeInfo){ .classSize    = sizeof(GUM_ManagerClass),
                                                       .pFnClassInit = GUM_ManagerClass_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC | GBL_TYPE_FLAG_CLASS_PINNED);
    }

    return type;
}
