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
static bool       GUM_Manager_initialized_ = false;

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

static void resourceRelease_(GUM_IResource* pResource) {
    if (!pResource) return;

    const GBL_RESULT unloadResult = GUM_IRESOURCE_CLASSOF(pResource)->pFnUnload(pResource);
    if (unloadResult != GBL_RESULT_SUCCESS)
        GUM_LOG_ERROR("Backend failed to unload resource!");

    GBL_UNREF(pResource);
}

static void resourceDestructor_(const GblHashSet* pSet, void* pItem) {
    GBL_UNUSED(pSet);
    GUM_HashSetEntry* pEntry = pItem;
    resourceRelease_(pEntry->pResource);
    pEntry->pResource = nullptr;
}

static GBL_RESULT GUM_Manager_ensureInitialized_(void) {
    if (GUM_Manager_initialized_)
        return GBL_RESULT_SUCCESS;

    const GBL_RESULT result = GblHashSet_construct(&GUM_Manager_hashSet_,
                                                   sizeof(GUM_HashSetEntry),
                                                   resourceHasher_,
                                                   resourceComparator_,
                                                   resourceDestructor_);
    if (result != GBL_RESULT_SUCCESS)
        return result;

    if (!getcwd(GUM_Manager_currentPath_, sizeof(GUM_Manager_currentPath_))) {
        GblHashSet_destruct(&GUM_Manager_hashSet_);
        GUM_Manager_hashSet_ = (GblHashSet){ 0 };
        GUM_Manager_currentPath_[0] = '\0';
        return GBL_RESULT_ERROR_INTERNAL;
    }

    GUM_Manager_initialized_ = true;
    return GBL_RESULT_SUCCESS;
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
        GBL_LOG_POP(1);
        return nullptr;
    }

    GUM_LOG_DEBUG("Path is %s", path);

    GblClass* managerClass = GblClass_refDefault(GUM_MANAGER_TYPE);
    if (!managerClass || GUM_Manager_ensureInitialized_() != GBL_RESULT_SUCCESS) {
        GUM_LOG_ERROR("Failed to initialize resource manager!");
        if (managerClass) GblClass_unrefDefault(managerClass);
        GBL_LOG_POP(1);
        return nullptr;
    }

    GblStringBuffer stringBuffer = { 0 };
    bool stringBufferConstructed = false;
    GUM_HashSetEntry entry = { 0 };

    const GBL_RESULT constructResult = GblStringBuffer_construct(&stringBuffer, path);
    if (constructResult != GBL_RESULT_SUCCESS) {
        GUM_LOG_ERROR("Failed to construct resource path buffer!");
        goto end;
    }
    stringBufferConstructed = true;

    if (GblStringBuffer_prepend(&stringBuffer, "/") != GBL_RESULT_SUCCESS ||
        GblStringBuffer_prepend(&stringBuffer, GUM_Manager_currentPath_) != GBL_RESULT_SUCCESS) {
        GUM_LOG_ERROR("Failed to build resource path!");
        goto end;
    }

    GblStringRef* fullPath = GblStringBuffer_cString(&stringBuffer);
    GUM_LOG_DEBUG("Full path is %s", fullPath);

    entry.quark = GblQuark_fromString(fullPath);

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
        if (!entry.pResource) {
            GUM_LOG_ERROR("Failed to allocate resource wrapper!");
            GBL_SCOPE_EXIT;
        }

        const GBL_RESULT loadResult = GUM_IRESOURCE_CLASSOF(entry.pResource)->pFnLoad(entry.pResource, fullPath);
        if (loadResult != GBL_RESULT_SUCCESS) {
            GUM_LOG_ERROR("Backend failed to load resource!");
            GBL_UNREF(entry.pResource);
            entry.pResource = nullptr;
            GBL_SCOPE_EXIT;
        }

        GUM_IRESOURCE_CLASSOF(entry.pResource)->pFnSetQuark(entry.pResource, entry.quark);
        if (!GblHashSet_insert(&GUM_Manager_hashSet_, &entry)) {
            GUM_LOG_ERROR("Failed to cache loaded resource!");
            resourceRelease_(entry.pResource);
            entry.pResource = nullptr;
            GBL_SCOPE_EXIT;
        }

        GUM_LOG_DEBUG("Resource loaded successfuly!");
    }

end:
    if (stringBufferConstructed)
        GblStringBuffer_destruct(&stringBuffer);
    GblClass_unrefDefault(managerClass);
    GUM_LOG_POP(1);

    if (entry.pResource) return GUM_IResource_ref(entry.pResource);
    return nullptr;
}

GBL_EXPORT void GUM_Manager_unload(GUM_IResource* pResource) {
    GUM_LOG_DEBUG_SCOPE("GUM_Manager_unload() called...") {
        if (!pResource) {
            GUM_LOG_ERROR("Resource passed to GUM_Manager_unload() is null");
            GBL_SCOPE_EXIT;
        }

        if (!GUM_Manager_initialized_) {
            GUM_LOG_ERROR("Attempted to unload a resource while the manager is not initialized!");
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

            /* One manager-owned ref plus at most one caller-owned ref may remain.
             * This preserves the existing unload contract without releasing the
             * manager ref before we finish using the resource object. */
            if (GblBox_refCount(GBL_BOX(pResource)) > 2) {
                GUM_LOG_ERROR("Attempted to unload a resource that is still being used!");
                GBL_SCOPE_EXIT;
            }

            GUM_LOG_DEBUG("No additional references left! Unloading...");
            GblHashSet_erase(&GUM_Manager_hashSet_, (const void*)&entry);
        }
    }
}

void GUM_Manager_deinit(void) {
    if (!GUM_Manager_initialized_) return;

    GblHashSet_destruct(&GUM_Manager_hashSet_);
    GUM_Manager_hashSet_ = (GblHashSet){ 0 };
    GUM_Manager_currentPath_[0] = '\0';
    GUM_Manager_initialized_ = false;
}

GBL_RESULT GUM_ManagerClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pClass, pData);
    return GUM_Manager_ensureInitialized_();
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
