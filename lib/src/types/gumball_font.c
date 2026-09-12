#include <gumball/types/gumball_font.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/core/gumball_manager.h>
#include <gimbal/meta/instances/gimbal_box.h>

#include "../ifaces/gumball_iresource_.h"

#ifndef GUM_DEFAULT_FONT_PATH
#error "GUM_DEFAULT_FONT_PATH must be provided by the build system"
#endif

static GUM_Font* pDefaultFont_ = nullptr;
static bool defaultFontFailureLogged_ = false;

static GBL_RESULT GUM_Font_load_(GUM_IResource* pResource, GblStringRef* pPath) {
    GBL_RESULT result = GUM_Backend_Font_load(pResource, pPath);
    if (!GBL_RESULT_SUCCESS(result))
        return result;

    result = GUM_IResource_setPath_(pResource, pPath);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        GUM_Backend_Font_unload(pResource);
    return result;
}

static GBL_RESULT GUM_Font_unload_(GUM_IResource* pResource) {
    const GBL_RESULT result = GUM_Backend_Font_unload(pResource);
    GUM_IResource_setPath_(pResource, nullptr);
    return result;
}

static GBL_RESULT GUM_Font_GblBox_destructor_(GblBox* pBox) {
    GUM_IResource* pResource = GUM_IRESOURCE(pBox);
    GBL_RESULT result = GUM_IRESOURCE_CLASSOF(pResource)->pFnUnload(pResource);

    GblBoxClass* pBoxClass = GBL_BOX_CLASS(GblClass_weakRefDefault(GBL_BOX_TYPE));
    const GBL_RESULT parentResult = pBoxClass->pFnDestructor(pBox);
    return GBL_RESULT_SUCCESS(result) ? parentResult : result;
}

GBL_RESULT GUM_FontClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    GUM_IRESOURCE_CLASS(pClass)->pFnLoad   = GUM_Font_load_;
    GUM_IRESOURCE_CLASS(pClass)->pFnUnload = GUM_Font_unload_;
    GBL_BOX_CLASS(pClass)->pFnDestructor   = GUM_Font_GblBox_destructor_;

    return GBL_RESULT_SUCCESS;
}

GUM_Font* GUM_Font_default(void) {
    return pDefaultFont_;
}

void GUM_Font_setDefault(GUM_Font* pFont) {
    if (pDefaultFont_ == pFont) return;

    if (pFont)
        GUM_IResource_ref(GUM_IRESOURCE(pFont));

    // Publish first because unloading the previous font may re-enter this API.
    GUM_Font* pPrevious = pDefaultFont_;
    pDefaultFont_ = pFont;

    if (pPrevious)
        GUM_IResource_unref(GUM_IRESOURCE(pPrevious));
}

GUM_Font* GUM_Backend_Font_default(void) {
    GUM_Font* pDefault = GUM_Font_default();
    if (pDefault)
        return pDefault;

    GUM_IResource* pResource = nullptr;
    const GBL_RESULT result = GUM_Manager_loadEx(GUM_DEFAULT_FONT_PATH, &pResource);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        if (!defaultFontFailureLogged_) {
            GUM_LOG_ERROR("Failed to load bundled default font '%s': %s",
                          GUM_DEFAULT_FONT_PATH,
                          gblResultString(result));
            defaultFontFailureLogged_ = true;
        }
        return nullptr;
    }

    defaultFontFailureLogged_ = false;
    GUM_Font* pFont = GUM_FONT(pResource);
    // Manager keeps the cache reference; the resolved default is borrowed.
    GUM_IResource_unref(pResource);
    return pFont;
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

        const GBL_RESULT result = GblVariant_registerConverter(type,
                                                               GBL_STRING_TYPE,
                                                               GUM_IResource_convertString_);
        GBL_ASSERT(GBL_RESULT_SUCCESS(result));
        GBL_UNUSED(result);
    }

    return type;
}
