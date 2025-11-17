#include "gimbal/meta/instances/gimbal_box.h"
#include <gumball/types/gumball_texture.h>
#include <gimbal/utils/gimbal_byte_array.h>

GBL_RESULT GUM_Texture_init_(GblInstance* pInstance) {
    GUM_Texture* pTexture = GUM_TEXTURE(pInstance);

    // GblByteArray = GblByteArray_create();

    GblBox_setField(GBL_BOX(pInstance), GblQuark_fromStatic("GUM_Resource_byteArray"), 0);
    GblBox_setField(GBL_BOX(pInstance), GblQuark_fromStatic("GUM_Resource_quark"), 0);
    GblBox_setField(GBL_BOX(pInstance), GblQuark_fromStatic("GUM_Resource_type"), (uintptr_t)GUM_TEXTURE_TYPE);

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Texture_create_(GUM_IResource *pResource, GblByteArray **ppByteArray, GblQuark quark, GblStringRef *extension) {
    GUM_Texture* pSelf = GUM_TEXTURE(pResource);
    GblBox_setField(GBL_BOX(pSelf), GblQuark_fromStatic("GUM_Resource_byteArray"), (uintptr_t)ppByteArray);
    GblBox_setField(GBL_BOX(pSelf), GblQuark_fromStatic("GUM_Resource_quark"), (uintptr_t)quark);
    GblBox_setField(GBL_BOX(pSelf), GblQuark_fromStatic("GUM_Resource_extension"), (uintptr_t)extension);
    GblBox_setField(GBL_BOX(pSelf), GblQuark_fromStatic("GUM_Resource_type"), (uintptr_t)GUM_TEXTURE_TYPE);
    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_TextureClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_TEXTURE_TYPE)) {
        GblQuark_internStatic("GUM_Resource_byteArray");
        GblQuark_internStatic("GUM_Resource_quark");
        GblQuark_internStatic("GUM_Resource_type");
    }

    GUM_IRESOURCE_CLASS(pClass)->pFnCreate = GUM_Texture_create_;

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_TextureClass_final_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    return GBL_RESULT_SUCCESS;
}

GblType GUM_Texture_type(void) {
    static GblType type = GBL_INVALID_TYPE;
    static GblInterfaceImpl ifaceEntries[] = {
        {
            .interfaceType = GBL_INVALID_TYPE,
            .classOffset   = offsetof(GUM_TextureClass, GUM_IResourceImpl)
        }
    };

	if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        ifaceEntries[0].interfaceType = GUM_IRESOURCE_TYPE;
		type = GblType_register(GblQuark_internStatic("GUM_Texture"),
								GBL_BOX_TYPE,
								&(static GblTypeInfo){.classSize        = sizeof(GUM_TextureClass),
                                                      .instanceSize     = sizeof(GUM_Texture),
                                                      .pFnClassInit     = GUM_TextureClass_init_,
													  .pFnInstanceInit  = GUM_Texture_init_,
                                                      .interfaceCount   = 1,
                                                      .pInterfaceImpls  = ifaceEntries,
													  .pFnClassFinal    = GUM_TextureClass_final_},
								GBL_TYPE_FLAG_TYPEINFO_STATIC);
	}

    return type;
}
