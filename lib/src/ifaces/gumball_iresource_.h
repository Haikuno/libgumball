#ifndef GUM_IRESOURCE_PRIVATE_H
#define GUM_IRESOURCE_PRIVATE_H

#include <gumball/ifaces/gumball_iresource.h>
#include <stdint.h>

#define GUM_IRESOURCE_VALUE_FIELD_NAME_ "GUM_IResource_value"

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

#endif // GUM_IRESOURCE_PRIVATE_H
