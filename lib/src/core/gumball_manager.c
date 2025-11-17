#include <gumball/core/gumball_manager.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/types/gumball_texture.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <unistd.h>

// TODO: make these private variables!
static GblHashSet GUM_Manager_hashSet_;
static char GUM_Manager_currentPath_[1024];

typedef struct GUM_HashSetEntry {
    GUM_IResource *pResource;
    GblQuark quark;
} GUM_HashSetEntry;

static GblHash resourceHasher_(const GblHashSet *pSet, const void *pItem) {
    GBL_UNUSED(pSet);
    GUM_HashSetEntry *pEntry = (GUM_HashSetEntry*)pItem;
    return gblHash(&pEntry->quark, sizeof(GblQuark));
}

static GblBool resourceComparator_(const GblHashSet *pSelf, const void *pEntry1, const void *pEntry2) {
    GBL_UNUSED(pSelf);
    const GUM_HashSetEntry *pResEntry1 = (const GUM_HashSetEntry*)pEntry1;
    const GUM_HashSetEntry *pResEntry2 = (const GUM_HashSetEntry*)pEntry2;
    return pResEntry1->quark == pResEntry2->quark;
}

static bool isExtension(const GblStringView path, const GblStringRef **extensions, GblStringRef **extension) {
    for (size_t i = 0; i < sizeof(extensions) / sizeof(extensions[0]); i++) {
        if (GblStringView_endsWith(path, extensions[i])) {
            *extension = extensions[i];
            return true;
        }
    }

    return false;
}

GUM_IResource *GUM_Manager_load(GblStringRef *path) {
    GUM_LOG_DEBUG_PUSH("GUM_Manager_load() called...");

    if (!path) {
        GUM_LOG_ERROR("Path passed to GUM_Manager_load() is null");
        return nullptr;
    }

    GUM_LOG_DEBUG("Path is %s", path);


    GblClass *managerClass = GblClass_refDefault(GUM_MANAGER_TYPE);
    GUM_IResource *pResource = nullptr;
    GblStringBuffer stringBuffer;

    GUM_LOG_DEBUG_SCOPE("Creating string buffer...") {
        if (!GblStringBuffer_construct(&stringBuffer, path)) {
            GUM_LOG_ERROR("Failed to construct string buffer!");
        }
        GUM_LOG_DEBUG("String buffer created successfuly!");
    }

    GblStringBuffer_prepend(&stringBuffer, "/");
    GblStringBuffer_prepend(&stringBuffer, GUM_Manager_currentPath_);
    GblStringRef *fullPath = GblStringBuffer_cString(&stringBuffer);
    GUM_LOG_DEBUG("Full path is %s", fullPath);
    GblQuark quark = GblQuark_fromString(fullPath);

    GUM_HashSetEntry entry = {
        .pResource = pResource,
        .quark = quark
    };

    if (GblHashSet_contains(&GUM_Manager_hashSet_, (const void*)&entry)) {
        GUM_LOG_DEBUG("Resource was already loaded! Returning a ref");
        entry = *(GUM_HashSetEntry*)GblHashSet_at(&GUM_Manager_hashSet_, &entry);
        goto end;
    }

    GUM_LOG_DEBUG_SCOPE("Resource was not loaded before! Loading...") {
        FILE *file;
        GblByteArray *pData;

        GUM_LOG_DEBUG_SCOPE("Opening file...") {
            GBL_SCOPE(file = fopen(fullPath, "rb"), fclose(file)) {
                if (!file) {
                    GUM_LOG_ERROR("Failed to open file %s!", fullPath);
                    GUM_LOG_POP(1);
                    GBL_SCOPE_EXIT;
                }

                fseek(file, 0, SEEK_END);
                long file_size = ftell(file);
                fseek(file, 0, SEEK_SET);
                pData = GblByteArray_create(file_size);
                fread(GblByteArray_data(pData), 1, file_size, file);
            }

            if (!pData) {
                GUM_LOG_ERROR("Array data is null for file %s!", fullPath);
                GUM_LOG_POP(1);
                GBL_SCOPE_EXIT;
            }

            GUM_LOG_DEBUG("File opened successfuly!");
        }


        GblStringView stringView = GblStringView_fromString(fullPath);
        GblStringRef *extension = nullptr;

        // TODO: supported extensions should be backend specific
        static const GblStringRef *texture_extensions[] = {
            ".png", ".bmp", ".tga", ".jpg", ".gif", ".hdr", ".pic",
            ".psd", ".dds", ".ktx", ".ktx2", ".pkm", ".pvr", ".astc"
        };
        // TODO: other resource types

        GUM_LOG_DEBUG_SCOPE("Checking for resource type...") {

            GUM_LOG_DEBUG_SCOPE("Checking for texture...") {
                if (isExtension(stringView, texture_extensions, &extension)) {
                    GUM_LOG_DEBUG_SCOPE("Resource is a texture! loading...") {
                        GUM_Texture *texture = GUM_TEXTURE(GblBox_create(GUM_TEXTURE_TYPE));
                        GUM_IRESOURCE_CLASSOF(texture)->pFnCreate(GUM_IRESOURCE(texture), &pData, quark, extension);
                        GUM_Texture_loadFromBytes(texture);
                        pResource = GUM_IRESOURCE(texture);
                        entry.pResource = pResource;
                        GblHashSet_insert(&GUM_Manager_hashSet_, (const void*)&entry);
                        GUM_LOG_DEBUG("Texture loaded successfuly!");
                    }
                }
            }
        }
    }

    end:
    GUM_LOG_POP(1);
    GblClass_unrefDefault(managerClass);
    GblStringBuffer_destruct(&stringBuffer);
    return GUM_IResource_ref(entry.pResource);
}

void GUM_Manager_unload(GUM_IResource *pResource) {
    GUM_LOG_DEBUG_SCOPE("GUM_Manager_unload() called...") {
        if (!pResource) {
            GUM_LOG_ERROR("Resource passed to GUM_Manager_unload() is null");
            GBL_SCOPE_EXIT;
        }

        GblQuark quark = (GblQuark)GblBox_field(GBL_BOX(pResource), GblQuark_fromStatic("GUM_Resource_quark"));

        GUM_HashSetEntry entry = {
            .pResource = pResource,
            .quark = quark
        };

        GUM_LOG_DEBUG_SCOPE("Checking if the resource is loaded...") {
            if (!GblHashSet_contains(&GUM_Manager_hashSet_, (const void*)&entry)) {
                GUM_LOG_ERROR("Attempted to unload a resource that was not loaded before!");
                GBL_SCOPE_EXIT;
            }

            GUM_LOG_DEBUG_SCOPE("The resource is currently loaded! Unreffing...") {
                GblHashSet_erase(&GUM_Manager_hashSet_, (const void*)&entry);
                GUM_IResource_unref(pResource);

                GUM_LOG_DEBUG_SCOPE("Checking if the resource is still being used...") {
                    if(GblBox_refCount(GBL_BOX(pResource)) > 1) {
                        GUM_LOG_ERROR("Attempted to unload a resource that is still being used!");
                        GBL_SCOPE_EXIT;
                    }

                    GUM_LOG_DEBUG("No references left! Checking resource type...");

                    GblType type = (GblType)GblBox_field(GBL_BOX(pResource), GblQuark_fromStatic("GUM_Resource_type"));

                    if (type == GUM_TEXTURE_TYPE) {
                        GUM_LOG_DEBUG_SCOPE("Resource is a texture! Unloading...") {
                            GUM_Texture_unload(GUM_TEXTURE(pResource));
                            GblHashSet_erase(&GUM_Manager_hashSet_, (const void*)&entry);
                            GUM_LOG_DEBUG("Texture unloaded successfuly!");
                        }
                    }
                }
            }
        }
    }
}

GBL_RESULT GUM_ManagerClass_init_(GblClass* pClass, const void* pData) {
	GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_MANAGER_TYPE)) {
        GblHashSet_construct(&GUM_Manager_hashSet_,
                             sizeof(GUM_HashSetEntry),
                             resourceHasher_,
                             resourceComparator_);
        getcwd(GUM_Manager_currentPath_, 1024);
    }

	return GBL_RESULT_SUCCESS;
}

GblType GUM_Manager_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Manager"),
                                GBL_STATIC_CLASS_TYPE,
                                &(static GblTypeInfo){.classSize = sizeof(GUM_ManagerClass),
                                                      .pFnClassInit = GUM_ManagerClass_init_},
                                GBL_TYPE_FLAG_TYPEINFO_STATIC | GBL_TYPE_FLAG_CLASS_PINNED);
    }

    return type;
}