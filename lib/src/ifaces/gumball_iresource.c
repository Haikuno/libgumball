#include <gumball/ifaces/gumball_iresource.h>

#include "gumball_iresource_.h"

#include <string.h>

static GBL_RESULT GUM_IResource_pathDtor_(const GblArrayMap* pMap,
                                          uintptr_t key,
                                          void* pValue) {
    GBL_UNUSED(pMap, key);
    GblStringRef_unref(pValue);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GUM_IResource* GUM_IResource_ref(GUM_IResource* pResource) {
    return pResource ? GUM_IRESOURCE(GBL_REF(pResource)) : nullptr;
}

GBL_EXPORT GblRefCount GUM_IResource_unref(GUM_IResource* pResource) {
    return pResource ? GBL_UNREF(pResource) : 0;
}

GBL_RESULT GUM_IResource_setPath_(GUM_IResource* pResource, GblStringRef* pPath) {
    if (!pResource)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    const GblQuark key = GblQuark_fromStatic(GUM_IRESOURCE_PATH_FIELD_NAME_);
    if (!pPath) {
        GblBox_clearField(GBL_BOX(pResource), key);
        return GBL_RESULT_SUCCESS;
    }

    GblStringRef* pNewPath = GblStringRef_create(pPath);
    if (!pNewPath)
        return GBL_RESULT_ERROR_MEM_ALLOC;

    const GBL_RESULT result = GblBox_setField(GBL_BOX(pResource),
                                              key,
                                              (uintptr_t)pNewPath,
                                              GUM_IResource_pathDtor_);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        GblStringRef_unref(pNewPath);
    return result;
}

GBL_RESULT GUM_IResource_convertString_(const GblVariant* pValue, GblVariant* pString) {
    if (!pValue || !pString)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GblBox* pBox = GblVariant_boxPeek(pValue);
    if (!pBox)
        return GblVariant_setString(pString, "(null)");

    GUM_IResource* pResource = GUM_IRESOURCE(pBox);
    const char* pName = GUM_IResource_path_(pResource);
    if (pName) {
        const char* pSlash = strrchr(pName, '/');
        const char* pBackslash = strrchr(pName, '\\');
        if (!pSlash || (pBackslash && pBackslash > pSlash))
            pSlash = pBackslash;
        if (pSlash && pSlash[1])
            pName = pSlash + 1;
    }

    return GblVariant_setString(pString,
                                pName && pName[0] ? pName
                                                  : GblType_name(GblVariant_typeOf(pValue)));
}

GblType GUM_IResource_type(void) {
    static GblType type           = GBL_INVALID_TYPE;
    static GblType dependencies[] = { GBL_INVALID_TYPE };

    if GBL_UNLIKELY (type == GBL_INVALID_TYPE) {
        dependencies[0] = GBL_BOX_TYPE;
        type = GblType_register(GblQuark_internStatic("GUM_IResource"),
                                GBL_INTERFACE_TYPE,
                                &(static GblTypeInfo){ .classSize        = sizeof(GUM_IResourceClass),
                                                       .pDependencies    = dependencies,
                                                       .dependencyCount = 1 },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
