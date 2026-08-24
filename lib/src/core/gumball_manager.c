#include <gumball/core/gumball_manager.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_logger.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/strings/gimbal_string_ref.h>

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <ctype.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

#define GUM_MANAGER_(self) (GBL_PRIVATE(GUM_Manager, self))
#define GUM_MANAGER_MODULE_NAME "GUM_Manager"

typedef struct GUM_HashSetEntry {
    GUM_IResource* pResource;
    GblStringRef*  pPath;
    GblHash        hash;
} GUM_HashSetEntry;

typedef struct GUM_Manager_ {
    GblHashSet    resources;
    GblStringRef* pBasePath;
    bool          initialized;
    bool          unloading;
} GUM_Manager_;

static GblHash resourceHasher_(const GblHashSet* pSet, const void* pItem) {
    GBL_UNUSED(pSet);
    return ((const GUM_HashSetEntry*)pItem)->hash;
}

static GblBool resourceComparator_(const GblHashSet* pSet, const void* pEntry1, const void* pEntry2) {
    GBL_UNUSED(pSet);
    const GUM_HashSetEntry* pLeft  = pEntry1;
    const GUM_HashSetEntry* pRight = pEntry2;
    return pLeft->hash == pRight->hash && strcmp(pLeft->pPath, pRight->pPath) == 0;
}

static void resourceWarnExternalRefs_(const GUM_HashSetEntry* pEntry, const char* pAction) {
    if (!pEntry || !pEntry->pResource || !pAction)
        return;

    const GblRefCount refCount = GblBox_refCount(GBL_BOX(pEntry->pResource));
    if (refCount <= 2)
        return;

    const unsigned externalRefs = (unsigned)(refCount - 1);
    GUM_LOG_WARN("%s resource '%s' with %u external reference%s remaining.",
                 pAction,
                 pEntry->pPath ? pEntry->pPath : "(unknown)",
                 externalRefs,
                 externalRefs == 1 ? "" : "s");
}

static void resourceDestructor_(const GblHashSet* pSet, void* pItem) {
    GBL_UNUSED(pSet);
    GUM_HashSetEntry* pEntry = pItem;
    const GUM_HashSetEntry entry = *pEntry;
    *pEntry = (GUM_HashSetEntry){ 0 };

    GUM_IResource_unref(entry.pResource);
    GblStringRef_unref(entry.pPath);
}

static GUM_HashSetEntry* resourceFind_(GUM_Manager_* pManager,
                                       const char* pPath,
                                       GblHash hash) {
    if (!pManager || !pManager->initialized || !pPath)
        return nullptr;

    const GUM_HashSetEntry key = {
        .pPath = (GblStringRef*)pPath,
        .hash  = hash
    };
    return GblHashSet_get(&pManager->resources, &key);
}

static GUM_HashSetEntry* resourceFindByPointer_(GUM_Manager_* pManager,
                                                const GUM_IResource* pResource) {
    if (!pManager || !pManager->initialized || !pResource)
        return nullptr;

    const size_t bucketCount = GblHashSet_bucketCount(&pManager->resources);
    for (size_t i = 0; i < bucketCount; ++i) {
        GUM_HashSetEntry* pEntry = GblHashSet_probe(&pManager->resources, i);
        if (pEntry && pEntry->pResource == pResource)
            return pEntry;
    }
    return nullptr;
}

static GBL_RESULT currentPath_(char** ppPath) {
    if (!ppPath)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    *ppPath = nullptr;

    size_t capacity = 256;
    for (;;) {
#ifdef _WIN32
        if (capacity > INT_MAX)
            return GBL_RESULT_ERROR_INTERNAL;
#endif
        char* pPath = malloc(capacity);
        if (!pPath)
            return GBL_RESULT_ERROR_MEM_ALLOC;

        errno = 0;
#ifdef _WIN32
        if (_getcwd(pPath, (int)capacity))
#else
        if (getcwd(pPath, capacity))
#endif
        {
            *ppPath = pPath;
            return GBL_RESULT_SUCCESS;
        }

        const int error = errno;
        free(pPath);
        if (error != ERANGE)
            return error == ENOMEM ? GBL_RESULT_ERROR_MEM_ALLOC : GBL_RESULT_ERROR_INTERNAL;
        if (capacity > SIZE_MAX / 2)
            return GBL_RESULT_ERROR_MEM_ALLOC;
        capacity *= 2;
    }
}

#ifndef _WIN32
static GBL_RESULT normalizePosixPath_(const char* pBasePath,
                                      const char* pPath,
                                      char** ppNormalized) {
    if (!ppNormalized)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    *ppNormalized = nullptr;
    if (!pBasePath || !pPath || !pPath[0])
        return GBL_RESULT_ERROR_INVALID_ARG;

    const bool   absolute = pPath[0] == '/';
    const size_t baseLen  = absolute ? 0 : strlen(pBasePath);
    const size_t pathLen  = strlen(pPath);
    if (baseLen > SIZE_MAX - pathLen - 2)
        return GBL_RESULT_ERROR_MEM_ALLOC;

    const size_t joinedCapacity = baseLen + pathLen + 2;
    char* pJoined = malloc(joinedCapacity);
    if (!pJoined)
        return GBL_RESULT_ERROR_MEM_ALLOC;

    if (absolute)
        memcpy(pJoined, pPath, pathLen + 1);
    else {
        memcpy(pJoined, pBasePath, baseLen);
        pJoined[baseLen] = '/';
        memcpy(pJoined + baseLen + 1, pPath, pathLen + 1);
    }

    const size_t joinedLen = strlen(pJoined);
    char* pNormalized = malloc(joinedLen + 2);
    if (!pNormalized) {
        free(pJoined);
        return GBL_RESULT_ERROR_MEM_ALLOC;
    }

    size_t read  = 0;
    size_t write = 0;
    pNormalized[write++] = '/';

    while (pJoined[read] == '/')
        ++read;

    while (pJoined[read]) {
        const size_t segmentStart = read;
        while (pJoined[read] && pJoined[read] != '/')
            ++read;
        const size_t segmentLen = read - segmentStart;

        if (segmentLen == 1 && pJoined[segmentStart] == '.') {
            /* lexical no-op */
        } else if (segmentLen == 2
                && pJoined[segmentStart]     == '.'
                && pJoined[segmentStart + 1] == '.') {
            if (write > 1) {
                while (write > 1 && pNormalized[write - 1] != '/')
                    --write;
                if (write > 1)
                    --write;
            }
        } else if (segmentLen) {
            if (write > 1)
                pNormalized[write++] = '/';
            memcpy(pNormalized + write, pJoined + segmentStart, segmentLen);
            write += segmentLen;
        }

        while (pJoined[read] == '/')
            ++read;
    }

    pNormalized[write] = '\0';
    free(pJoined);
    *ppNormalized = pNormalized;
    return GBL_RESULT_SUCCESS;
}
#else
static bool windowsSeparator_(char value) {
    return value == '/' || value == '\\';
}

static bool windowsDrivePrefix_(const char* pPath) {
    return pPath && isalpha((unsigned char)pPath[0]) && pPath[1] == ':';
}

static bool windowsPathFullyQualified_(const char* pPath) {
    if (!pPath || !pPath[0])
        return false;
    if (windowsDrivePrefix_(pPath) && windowsSeparator_(pPath[2]))
        return true;
    return windowsSeparator_(pPath[0]) && windowsSeparator_(pPath[1]);
}

static bool windowsPathPartiallyQualified_(const char* pPath) {
    if (!pPath || !pPath[0])
        return false;
    if (windowsDrivePrefix_(pPath) && !windowsSeparator_(pPath[2]))
        return true;
    return windowsSeparator_(pPath[0]) && !windowsSeparator_(pPath[1]);
}

static GBL_RESULT windowsFullPath_(const char* pPath, char** ppNormalized) {
    errno = 0;
    char* pNormalized = _fullpath(nullptr, pPath, 0);
    if (!pNormalized) {
        if (errno == ENOMEM)
            return GBL_RESULT_ERROR_MEM_ALLOC;
        if (errno == EINVAL)
            return GBL_RESULT_ERROR_INVALID_ARG;
        return GBL_RESULT_ERROR_INTERNAL;
    }

    *ppNormalized = pNormalized;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT normalizeWindowsPath_(const char* pBasePath,
                                        const char* pPath,
                                        char** ppNormalized) {
    if (!ppNormalized)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    *ppNormalized = nullptr;
    if (!pBasePath || !pPath || !pPath[0])
        return GBL_RESULT_ERROR_INVALID_ARG;

    if (windowsPathPartiallyQualified_(pPath))
        return GBL_RESULT_UNSUPPORTED;

    if (windowsPathFullyQualified_(pPath))
        return windowsFullPath_(pPath, ppNormalized);

    const size_t baseLen = strlen(pBasePath);
    const size_t pathLen = strlen(pPath);
    if (baseLen > SIZE_MAX - pathLen - 2)
        return GBL_RESULT_ERROR_MEM_ALLOC;

    char* pJoined = malloc(baseLen + pathLen + 2);
    if (!pJoined)
        return GBL_RESULT_ERROR_MEM_ALLOC;

    memcpy(pJoined, pBasePath, baseLen);
    pJoined[baseLen] = '\\';
    memcpy(pJoined + baseLen + 1, pPath, pathLen + 1);

    const GBL_RESULT result = windowsFullPath_(pJoined, ppNormalized);
    free(pJoined);
    return result;
}
#endif

static GBL_RESULT normalizeBasePath_(const char* pPath, char** ppNormalized) {
    if (!ppNormalized)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    *ppNormalized = nullptr;
    if (!pPath || !pPath[0])
        return GBL_RESULT_ERROR_INVALID_ARG;

    char* pCurrentPath = nullptr;
    GBL_RESULT result = currentPath_(&pCurrentPath);
    if (!GBL_RESULT_SUCCESS(result))
        return result;

#ifdef _WIN32
    result = normalizeWindowsPath_(pCurrentPath, pPath, ppNormalized);
#else
    result = normalizePosixPath_(pCurrentPath, pPath, ppNormalized);
#endif

    free(pCurrentPath);
    return result;
}

static GBL_RESULT normalizedPath_(const GUM_Manager_* pManager,
                                  const char* pPath,
                                  char** ppNormalized) {
    if (!ppNormalized)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    *ppNormalized = nullptr;
    if (!pManager || !pManager->pBasePath || !pPath || !pPath[0])
        return GBL_RESULT_ERROR_INVALID_ARG;

#ifdef _WIN32
    return normalizeWindowsPath_(pManager->pBasePath, pPath, ppNormalized);
#else
    return normalizePosixPath_(pManager->pBasePath, pPath, ppNormalized);
#endif
}

static GUM_Manager* GUM_Manager_active_(void) {
    GblModule* pModule = GblModule_find(GUM_MANAGER_MODULE_NAME);
    return pModule && GblType_check(GBL_TYPEOF(pModule), GUM_MANAGER_TYPE)
         ? GUM_MANAGER(pModule)
         : nullptr;
}

static GBL_RESULT GUM_Manager_start_(GUM_Manager** ppManager) {
    if (!ppManager)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    *ppManager = nullptr;

    GUM_Manager* pManager = GUM_Manager_active_();
    if (pManager) {
        GUM_Manager_* pManager_ = GUM_MANAGER_(pManager);
        if (pManager_->unloading) {
            GUM_LOG_WARN("Ignoring Manager operation while shutting down.");
            return GBL_RESULT_NOT_READY;
        }

        if (!GblModule_isLoaded(GBL_MODULE(pManager))) {
            const GBL_RESULT result = GblModule_use(GBL_MODULE(pManager));
            if (!GBL_RESULT_SUCCESS(result)) {
                if (GblModule_useCount(GBL_MODULE(pManager)))
                    GblModule_unuse(GBL_MODULE(pManager));
                return result;
            }
        }
        *ppManager = pManager;
        return GBL_RESULT_SUCCESS;
    }

    pManager = GUM_MANAGER(GBL_NEW(GUM_Manager));
    if (!pManager)
        return GBL_RESULT_ERROR_MEM_ALLOC;

    GBL_RESULT result = GblModule_register(GBL_MODULE(pManager));
    if (!GBL_RESULT_SUCCESS(result)) {
        GblModule_unref(GBL_MODULE(pManager));
        return result;
    }

    result = GblModule_use(GBL_MODULE(pManager));
    if (!GBL_RESULT_SUCCESS(result)) {
        if (GblModule_useCount(GBL_MODULE(pManager)))
            GblModule_unuse(GBL_MODULE(pManager));
        GblModule_unregister(GBL_MODULE(pManager));
        GblModule_unref(GBL_MODULE(pManager));
        return result;
    }

    // The registry owns the service object; the module use owns its runtime state.
    GblModule_unref(GBL_MODULE(pManager));
    pManager = GUM_Manager_active_();
    if (!pManager)
        return GBL_RESULT_ERROR_INTERNAL;

    *ppManager = pManager;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Manager_GblModule_load_(GblModule* pModule) {
    GUM_Manager_* pManager = GUM_MANAGER_(GUM_MANAGER(pModule));
    if (!pManager)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (pManager->unloading)
        return GBL_RESULT_NOT_READY;
    if (pManager->initialized)
        return GBL_RESULT_SUCCESS;

    GBL_RESULT result = GblHashSet_construct(&pManager->resources,
                                             sizeof(GUM_HashSetEntry),
                                             resourceHasher_,
                                             resourceComparator_,
                                             resourceDestructor_);
    if (!GBL_RESULT_SUCCESS(result))
        return result;

    char* pCurrentPath = nullptr;
    result = currentPath_(&pCurrentPath);
    if (!GBL_RESULT_SUCCESS(result)) {
        GblHashSet_destruct(&pManager->resources);
        pManager->resources = (GblHashSet){ 0 };
        return result;
    }

    pManager->pBasePath = GblStringRef_create(pCurrentPath);
    free(pCurrentPath);
    if (!pManager->pBasePath) {
        GblHashSet_destruct(&pManager->resources);
        pManager->resources = (GblHashSet){ 0 };
        return GBL_RESULT_ERROR_MEM_ALLOC;
    }

    pManager->initialized = true;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Manager_GblModule_unload_(GblModule* pModule) {
    GUM_Manager_* pManager = GUM_MANAGER_(GUM_MANAGER(pModule));
    if (!pManager || !pManager->initialized)
        return GBL_RESULT_SUCCESS;
    if (pManager->unloading)
        return GBL_RESULT_SUCCESS;

    const size_t bucketCount = GblHashSet_bucketCount(&pManager->resources);
    for (size_t i = 0; i < bucketCount; ++i) {
        GUM_HashSetEntry* pEntry = GblHashSet_probe(&pManager->resources, i);
        if (pEntry)
            resourceWarnExternalRefs_(pEntry, "Manager shutdown is releasing");
    }

    pManager->unloading   = true;
    pManager->initialized = false;

    const GBL_RESULT destructResult = GblHashSet_destruct(&pManager->resources);
    GBL_ASSERT(GBL_RESULT_SUCCESS(destructResult));
    GBL_UNUSED(destructResult);
    pManager->resources = (GblHashSet){ 0 };

    GblStringRef_unref(pManager->pBasePath);
    pManager->pBasePath = nullptr;
    pManager->unloading = false;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_Manager_loadEx(GblStringRef* pPath, GUM_IResource** ppResource) {
    if (!ppResource)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    *ppResource = nullptr;

    if (!pPath || !pPath[0])
        return GBL_RESULT_ERROR_INVALID_ARG;

    GUM_Manager* pManagerObject = nullptr;
    GBL_RESULT   result         = GUM_Manager_start_(&pManagerObject);
    if (!GBL_RESULT_SUCCESS(result))
        return result;

    GUM_Manager_* pManager = GUM_MANAGER_(pManagerObject);
    char* pNormalizedPath = nullptr;
    result = normalizedPath_(pManager, pPath, &pNormalizedPath);
    if (!GBL_RESULT_SUCCESS(result))
        return result;

    const GblType resourceType = GUM_Backend_resourceType(pNormalizedPath);
    if (resourceType == GBL_INVALID_TYPE) {
        free(pNormalizedPath);
        return GBL_RESULT_UNSUPPORTED;
    }

    const GblHash pathHash = gblHash(pNormalizedPath, strlen(pNormalizedPath));
    GUM_HashSetEntry* pCached = resourceFind_(pManager, pNormalizedPath, pathHash);
    if (pCached) {
        *ppResource = GUM_IResource_ref(pCached->pResource);
        free(pNormalizedPath);
        return *ppResource ? GBL_RESULT_SUCCESS : GBL_RESULT_ERROR_INTERNAL;
    }

    GUM_IResource* pResource = GUM_IRESOURCE(GblBox_create(resourceType));
    if (!pResource) {
        free(pNormalizedPath);
        return GBL_RESULT_ERROR_MEM_ALLOC;
    }

    result = GUM_IRESOURCE_CLASSOF(pResource)->pFnLoad(pResource, pNormalizedPath);
    if (GBL_RESULT_SUCCESS(result)) {
        GblStringRef* pCachedPath = GblStringRef_create(pNormalizedPath);
        if (!pCachedPath) {
            result = GBL_RESULT_ERROR_MEM_ALLOC;
        } else {
            const GUM_HashSetEntry entry = {
                .pResource = pResource,
                .pPath     = pCachedPath,
                .hash      = pathHash
            };
            if (!GblHashSet_insert(&pManager->resources, &entry)) {
                GblStringRef_unref(pCachedPath);
                result = GBL_RESULT_ERROR_MEM_ALLOC;
            }
        }
    }

    if (GBL_RESULT_SUCCESS(result))
        *ppResource = GUM_IResource_ref(pResource);
    else
        GUM_IResource_unref(pResource);

    free(pNormalizedPath);
    return result;
}

GBL_EXPORT GUM_IResource* GUM_Manager_load(GblStringRef* pPath) {
    GUM_IResource* pResource = nullptr;
    const GBL_RESULT result = GUM_Manager_loadEx(pPath, &pResource);
    if (!GBL_RESULT_SUCCESS(result))
        GUM_LOG_ERROR("Failed to load resource '%s': %s", pPath ? pPath : "(null)", gblResultString(result));
    return pResource;
}

GBL_EXPORT GBL_RESULT GUM_Manager_evict(GUM_IResource* pResource) {
    if (!pResource)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_Manager* pManagerObject = GUM_Manager_active_();
    if (!pManagerObject || !GblModule_isLoaded(GBL_MODULE(pManagerObject)))
        return GBL_RESULT_NOT_READY;

    GUM_Manager_* pManager = GUM_MANAGER_(pManagerObject);
    if (pManager->unloading) {
        GUM_LOG_WARN("Ignoring Manager eviction while shutting down.");
        return GBL_RESULT_NOT_READY;
    }

    GUM_HashSetEntry* pCached = resourceFindByPointer_(pManager, pResource);
    if (!pCached)
        return GBL_RESULT_NOT_FOUND;

    resourceWarnExternalRefs_(pCached, "Evicting");

    const GUM_HashSetEntry key = {
        .pPath = pCached->pPath,
        .hash  = pCached->hash
    };
    return GblHashSet_erase(&pManager->resources, &key)
         ? GBL_RESULT_SUCCESS
         : GBL_RESULT_NOT_FOUND;
}

GBL_EXPORT GBL_RESULT GUM_Manager_setBasePath(GblStringRef* pPath) {
    if (!pPath || !pPath[0])
        return GBL_RESULT_ERROR_INVALID_ARG;

    GUM_Manager* pManagerObject = nullptr;
    GBL_RESULT   result         = GUM_Manager_start_(&pManagerObject);
    if (!GBL_RESULT_SUCCESS(result))
        return result;

    char* pNormalizedPath = nullptr;
    result = normalizeBasePath_(pPath, &pNormalizedPath);
    if (!GBL_RESULT_SUCCESS(result))
        return result;

    GblStringRef* pBasePath = GblStringRef_create(pNormalizedPath);
    free(pNormalizedPath);
    if (!pBasePath)
        return GBL_RESULT_ERROR_MEM_ALLOC;

    GUM_Manager_* pManager = GUM_MANAGER_(pManagerObject);
    GblStringRef_unref(pManager->pBasePath);
    pManager->pBasePath = pBasePath;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblStringRef* GUM_Manager_basePath(void) {
    GUM_Manager* pManager = nullptr;
    const GBL_RESULT result = GUM_Manager_start_(&pManager);
    if (!GBL_RESULT_SUCCESS(result)) {
        GUM_LOG_ERROR("Failed to get Manager base path: %s", gblResultString(result));
        return nullptr;
    }

    return GUM_MANAGER_(pManager)->pBasePath;
}

GBL_EXPORT void GUM_Manager_deinit(void) {
    GUM_Manager* pManager = GUM_Manager_active_();
    if (!pManager)
        return;

    GblModule* pModule = GBL_MODULE(pManager);
    const GblRefCount useCount = GblModule_useCount(pModule);
    if (!GblModule_isLoaded(pModule) || useCount != 1) {
        GUM_LOG_ERROR("Manager shutdown invariant violated: expected exactly one framework-owned module use, got %u.",
                      useCount);
        GBL_ASSERT(GblModule_isLoaded(pModule) && useCount == 1);
        return;
    }

    const GBL_RESULT unloadResult = GblModule_unuse(pModule);
    GBL_ASSERT(GBL_RESULT_SUCCESS(unloadResult));
    if (!GBL_RESULT_SUCCESS(unloadResult))
        return;

    const GBL_RESULT unregisterResult = GblModule_unregister(pModule);
    GBL_ASSERT(GBL_RESULT_SUCCESS(unregisterResult));
}

static GBL_RESULT GUM_ManagerClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GblModuleClass* pModuleClass = GBL_MODULE_CLASS(pClass);
    pModuleClass->pFnLoad   = GUM_Manager_GblModule_load_;
    pModuleClass->pFnUnload = GUM_Manager_GblModule_unload_;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_Manager_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if GBL_UNLIKELY (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Manager"),
                                GBL_MODULE_TYPE,
                                &(static GblTypeInfo){ .classSize           = sizeof(GUM_ManagerClass),
                                                       .instanceSize        = sizeof(GUM_Manager),
                                                       .instancePrivateSize = sizeof(GUM_Manager_),
                                                       .pFnClassInit        = GUM_ManagerClass_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}