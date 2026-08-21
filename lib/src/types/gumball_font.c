#include <gumball/types/gumball_font.h>
#include <gumball/core/gumball_backend.h>
#include <gimbal/meta/instances/gimbal_box.h>
#include <gimbal/utils/gimbal_byte_array.h>

static GUM_Font* pDefaultFont_ = nullptr;

GBL_RESULT GUM_FontClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    GUM_IRESOURCE_CLASS(pClass)->pFnLoad   = GUM_Backend_Font_load;
    GUM_IRESOURCE_CLASS(pClass)->pFnUnload = GUM_Backend_Font_unload;

    return GBL_RESULT_SUCCESS;
}

GUM_Font* GUM_Font_default(void) {
    return pDefaultFont_;
}

void GUM_Font_setDefault(GUM_Font* pFont) {
    if (pDefaultFont_ == pFont) return;

    if (pFont)
        GUM_IResource_ref(GUM_IRESOURCE(pFont));

    if (pDefaultFont_)
        GUM_IResource_unref(GUM_IRESOURCE(pDefaultFont_));

    pDefaultFont_ = pFont;
}

GblType GUM_Font_type(void) {
    static GblType          type           = GBL_INVALID_TYPE;
    static GblInterfaceImpl ifaceEntries[] = {
        { .interfaceType = GBL_INVALID_TYPE, .classOffset = offsetof(GUM_FontClass, GUM_IResourceImpl) }
    };

    if GBL_UNLIKELY (type == GBL_INVALID_TYPE) {
        ifaceEntries[0].interfaceType = GUM_IRESOURCE_TYPE;

        type = GblType_register(GblQuark_internStatic("GUM_Font"),
                                GBL_BOX_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_FontClass),
                                                       .instanceSize    = sizeof(GUM_Font),
                                                       .pFnClassInit    = GUM_FontClass_init_,
                                                       .interfaceCount  = 1,
                                                       .pInterfaceImpls = ifaceEntries },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}