#include <gimbal/meta/instances/gimbal_box.h>
#include <gumball/types/gumball_texture.h>
#include <gumball/core/gumball_backend.h>

static GBL_RESULT GUM_Texture_GblBox_destructor_(GblBox* pBox) {
    GUM_IResource* pResource = GUM_IRESOURCE(pBox);
    GBL_RESULT result = GUM_IRESOURCE_CLASSOF(pResource)->pFnUnload(pResource);

    GblBoxClass* pBoxClass = GBL_BOX_CLASS(GblClass_weakRefDefault(GBL_BOX_TYPE));
    const GBL_RESULT parentResult = pBoxClass->pFnDestructor(pBox);
    return GBL_RESULT_SUCCESS(result) ? parentResult : result;
}

GBL_RESULT GUM_TextureClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    GUM_IRESOURCE_CLASS(pClass)->pFnLoad   = GUM_Backend_Texture_load;
    GUM_IRESOURCE_CLASS(pClass)->pFnUnload = GUM_Backend_Texture_unload;
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
    }

    return type;
}
