#include <gumball/devices/gumball_pointer.h>

static GBL_RESULT GUM_Pointer_init_(GblInstance* pInstance) {
    GUM_Pointer* pSelf = GUM_POINTER(pInstance);

    pSelf->position = (GUM_Vector2){ 0, 0 };
    pSelf->delta    = (GUM_Vector2){ 0, 0 };

    return GBL_RESULT_SUCCESS;
}

GblType GUM_Pointer_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Pointer"),
                                GUM_INPUTDEVICE_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_PointerClass),
                                                       .instanceSize    = sizeof(GUM_Pointer),
                                                       .pFnInstanceInit = GUM_Pointer_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
