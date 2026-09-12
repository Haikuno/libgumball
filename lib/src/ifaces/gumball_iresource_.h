#ifndef GUM_IRESOURCE_PRIVATE_H
#define GUM_IRESOURCE_PRIVATE_H

#include <gumball/ifaces/gumball_iresource.h>
#include <stdint.h>

#define GUM_IRESOURCE_VALUE_FIELD_NAME_ "GUM_IResource_value"
#define GUM_IRESOURCE_PATH_FIELD_NAME_  "GUM_IResource_path"

GBL_INLINE void* GUM_IResource_data_(const GUM_IResource* pResource) {
    return pResource ?
           (void*)GblBox_field(GBL_BOX(pResource),
                               GblQuark_fromStatic(GUM_IRESOURCE_VALUE_FIELD_NAME_)) :
           nullptr;
}

GBL_INLINE void GUM_IResource_setData_(GUM_IResource* pResource, void* pValue) {
    if (pResource)
        GblBox_setField(GBL_BOX(pResource),
                        GblQuark_fromStatic(GUM_IRESOURCE_VALUE_FIELD_NAME_),
                        (uintptr_t)pValue);
}

GBL_INLINE GblStringRef* GUM_IResource_path_(const GUM_IResource* pResource) {
    return pResource ?
           (GblStringRef*)GblBox_field(GBL_BOX(pResource),
                                       GblQuark_fromStatic(GUM_IRESOURCE_PATH_FIELD_NAME_)) :
           nullptr;
}

GBL_RESULT GUM_IResource_setPath_      (GUM_IResource* pResource, GblStringRef* pPath) GBL_NOEXCEPT;
GBL_RESULT GUM_IResource_convertString_(const GblVariant* pValue, GblVariant* pString) GBL_NOEXCEPT;

#endif // GUM_IRESOURCE_PRIVATE_H
