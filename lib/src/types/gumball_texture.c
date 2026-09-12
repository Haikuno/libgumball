#include <gimbal/meta/instances/gimbal_box.h>
#include <gumball/types/gumball_texture.h>
#include <gumball/core/gumball_backend.h>

#include "../ifaces/gumball_iresource_.h"

static GBL_RESULT GUM_Texture_load_(GUM_IResource* pResource, GblStringRef* pPath) {
    GBL_RESULT result = GUM_Backend_Texture_load(pResource, pPath);
    if (!GBL_RESULT_SUCCESS(result))
        return result;

    result = GUM_IResource_setPath_(pResource, pPath);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        GUM_Backend_Texture_unload(pResource);
    return result;
}

static GBL_RESULT GUM_Texture_unload_(GUM_IResource* pResource) {
    const GBL_RESULT result = GUM_Backend_Texture_unload(pResource);
    GUM_IResource_setPath_(pResource, nullptr);
    return result;
}

static GBL_RESULT GUM_Texture_GblBox_destructor_(GblBox* pBox) {
    GUM_IResource* pResource = GUM_IRESOURCE(pBox);
    GBL_RESULT result = GUM_IRESOURCE_CLASSOF(pResource)->pFnUnload(pResource);

    GblBoxClass* pBoxClass = GBL_BOX_CLASS(GblClass_weakRefDefault(GBL_BOX_TYPE));
    const GBL_RESULT parentResult = pBoxClass->pFnDestructor(pBox);
    return GBL_RESULT_SUCCESS(result) ? parentResult : result;
}

GBL_RESULT GUM_TextureClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    GUM_IRESOURCE_CLASS(pClass)->pFnLoad   = GUM_Texture_load_;
    GUM_IRESOURCE_CLASS(pClass)->pFnUnload = GUM_Texture_unload_;
    GBL_BOX_CLASS(pClass)->pFnDestructor   = GUM_Texture_GblBox_destructor_;

    return GBL_RESULT_SUCCESS;
}

GblType GUM_Texture_type(void) {
    static GblType          type           = GBL_INVALID_TYPE;
    static GblInterfaceImpl ifaceEntries[] = {
        { .interfaceType = GBL_INVALID_TYPE,
          .classOffset = offsetof(GUM_TextureClass, GUM_IResourceImpl) }
    };

    if GBL_UNLIKELY (type == GBL_INVALID_TYPE) {
        ifaceEntries[0].interfaceType = GUM_IRESOURCE_TYPE;

        type = GblType_register(GblQuark_internStatic("GUM_Texture"),
                                GBL_BOX_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_TextureClass),
                                                       .instanceSize    = sizeof(GUM_Texture),
                                                       .pFnClassInit    = GUM_TextureClass_init_,
                                                       .interfaceCount  = 1,
                                                       .pInterfaceImpls = ifaceEntries },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);

        const GBL_RESULT result = GblVariant_registerConverter(type,
                                                               GBL_STRING_TYPE,
                                                               GUM_IResource_convertString_);
        GBL_ASSERT(GBL_RESULT_SUCCESS(result));
        GBL_UNUSED(result);
    }

    return type;
}
