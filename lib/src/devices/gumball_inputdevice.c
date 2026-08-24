#include <gumball/devices/gumball_inputdevice.h>
#include <gumball/core/gumball_inputsystem.h>
#include <gumball/core/gumball_logger.h>

#include <gimbal/containers/gimbal_array_list.h>

#include "../elements/gumball_widget_.h"
#include "gumball_inputdevice_.h"

#define GUM_INPUTDEVICE_(self) (GBL_PRIVATE(GUM_InputDevice, self))

typedef struct GUM_InputDevice_ {
    GUM_InputDevice* pPrev;
    GUM_InputDevice* pNext;
    GblStringRef*    pDeviceName;
    GUM_Widget*      pFocusedWidget;
    bool             enabled;
    bool             focusRingEnabled;
    bool             baselinePending;
    bool             focusTransition;
} GUM_InputDevice_;

static GUM_InputDevice* pFirstDevice_ = nullptr;

GBL_EXPORT GblStringRef* GUM_InputDevice_name(const GUM_InputDevice* pSelf) {
    return pSelf ? GUM_INPUTDEVICE_(pSelf)->pDeviceName : nullptr;
}

GBL_EXPORT GUM_Widget* GUM_InputDevice_focusedWidget(const GUM_InputDevice* pSelf) {
    return pSelf ? GUM_INPUTDEVICE_(pSelf)->pFocusedWidget : nullptr;
}

bool GUM_InputDevice_enabled_(const GUM_InputDevice* pDevice) {
    return pDevice && GUM_INPUTDEVICE_(pDevice)->enabled;
}

bool GUM_InputDevice_focusRingEnabled_(const GUM_InputDevice* pDevice) {
    return pDevice && GUM_INPUTDEVICE_(pDevice)->focusRingEnabled;
}

bool GUM_InputDevice_baselinePending_(const GUM_InputDevice* pDevice) {
    return pDevice && GUM_INPUTDEVICE_(pDevice)->baselinePending;
}

bool GUM_InputDevice_takeBaseline_(GUM_InputDevice* pDevice) {
    if (!pDevice)
        return false;

    GUM_InputDevice_* pSelf_ = GUM_INPUTDEVICE_(pDevice);
    const bool pending = pSelf_->baselinePending;
    pSelf_->baselinePending = false;
    return pending;
}

void GUM_InputDevice_setEnabled_(GUM_InputDevice* pDevice, bool enabled) {
    if (pDevice)
        GUM_INPUTDEVICE_(pDevice)->enabled = enabled;
}

void GUM_InputDevice_setFocusRingEnabled_(GUM_InputDevice* pDevice, bool enabled) {
    if (pDevice)
        GUM_INPUTDEVICE_(pDevice)->focusRingEnabled = enabled;
}

void GUM_InputDevice_requestBaseline_(GUM_InputDevice* pDevice) {
    if (pDevice)
        GUM_INPUTDEVICE_(pDevice)->baselinePending = true;
}

void GUM_InputDevice_resetTransient_(GUM_InputDevice* pDevice) {
    if (!pDevice)
        return;

    pDevice->buttons = 0;
    pDevice->buttonsPrev = 0;
}

typedef struct GUM_InputDeviceSnapshot_ {
    GblArrayList     devices;
    GUM_InputDevice* stack[GUM_SNAPSHOT_INLINE_CAPACITY];
} GUM_InputDeviceSnapshot_;

static void GUM_InputDevice_snapshotRelease_(GUM_InputDeviceSnapshot_* pSnapshot) {
    const size_t count = GblArrayList_size(&pSnapshot->devices);
    for (size_t i = 0; i < count; ++i) {
        GUM_InputDevice* pDevice = *(GUM_InputDevice**)GblArrayList_at(&pSnapshot->devices, i);
        GblBox_unref(GBL_BOX(pDevice));
    }
    GblArrayList_destruct(&pSnapshot->devices);
}

GBL_RESULT GUM_InputDevice_foreach_(GblType deviceType,
                                    GUM_InputDeviceVisitFn_ pFnVisit,
                                    void* pClosure) {
    if (!pFnVisit)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (deviceType == GBL_INVALID_TYPE)
        return GBL_RESULT_ERROR_INVALID_ARG;

    GUM_InputDeviceSnapshot_ snapshot;
    GBL_RESULT result = GblArrayList_construct(&snapshot.devices,
                                               sizeof(GUM_InputDevice*),
                                               0,
                                               nullptr,
                                               sizeof(snapshot));
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    for (GUM_InputDevice* pDevice = pFirstDevice_; pDevice;
         pDevice = GUM_INPUTDEVICE_(pDevice)->pNext) {
        if (GblBox_isFinalizing(GBL_BOX(pDevice)) ||
            !GblType_check(GBL_TYPEOF(pDevice), deviceType))
            continue;

        GUM_InputDevice* pRetained = GUM_INPUTDEVICE(GblBox_ref(GBL_BOX(pDevice)));
        result = GblArrayList_pushBack(&snapshot.devices, &pRetained);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
            GblBox_unref(GBL_BOX(pRetained));
            GUM_InputDevice_snapshotRelease_(&snapshot);
            return result;
        }
    }

    const size_t count = GblArrayList_size(&snapshot.devices);
    for (size_t i = 0; i < count; ++i) {
        GUM_InputDevice* pDevice = *(GUM_InputDevice**)GblArrayList_at(&snapshot.devices, i);
        pFnVisit(pDevice, pClosure);
    }

    GUM_InputDevice_snapshotRelease_(&snapshot);
    return GBL_RESULT_SUCCESS;
}

void GUM_InputDevice_focusSet_(GUM_InputDevice* pDevice, GUM_Widget* pWidget) {
    if (pDevice)
        GUM_INPUTDEVICE_(pDevice)->pFocusedWidget = pWidget;
}

GBL_RESULT GUM_InputDevice_setName_(GUM_InputDevice* pDevice, const char* pName) {
    if (!pDevice || !pName)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GblStringRef* pNewName = GblStringRef_create(pName);
    if (!pNewName)
        return GBL_RESULT_ERROR_MEM_ALLOC;

    GUM_InputDevice_* pSelf_ = GUM_INPUTDEVICE_(pDevice);
    GblStringRef* pOldName = pSelf_->pDeviceName;
    pSelf_->pDeviceName = pNewName;
    GblStringRef_unref(pOldName);
    return GBL_RESULT_SUCCESS;
}

bool GUM_InputDevice_focusTransitionBegin_(GUM_InputDevice* pDevice) {
    if (!pDevice)
        return false;

    GUM_InputDevice_* pSelf_ = GUM_INPUTDEVICE_(pDevice);
    if (pSelf_->focusTransition) {
        GUM_LOG_WARN("Ignoring reentrant focus change on '%s'.",
                     pSelf_->pDeviceName ? pSelf_->pDeviceName : "UNKNOWN");
        return false;
    }

    pSelf_->focusTransition = true;
    return true;
}

void GUM_InputDevice_focusTransitionEnd_(GUM_InputDevice* pDevice) {
    if (!pDevice)
        return;

    GUM_InputDevice_* pSelf_ = GUM_INPUTDEVICE_(pDevice);
    GUM_Widget* pWidget = pSelf_->pFocusedWidget;
    if (pWidget)
        GblBox_unref(GBL_BOX(pWidget));

    pSelf_->focusTransition = false;

    // A focus callback may disable the device while the transition is active.
    if (!GUM_InputSystem_deviceEnabled(pDevice) && pSelf_->pFocusedWidget) {
        pSelf_->focusTransition = true;
        GUM_InputDevice_focusRelease_(pDevice);
        pSelf_->focusTransition = false;
    }
}

void GUM_InputDevice_focusRelease_(GUM_InputDevice* pDevice) {
    if (!pDevice)
        return;

    GUM_InputDevice_* pSelf_ = GUM_INPUTDEVICE_(pDevice);
    if (!pSelf_->pFocusedWidget)
        return;

    // Focus is non-owning; retain only across the focus-loss callback.
    GUM_Widget* pWidget = GUM_WIDGET(GblBox_ref(GBL_BOX(pSelf_->pFocusedWidget)));
    pSelf_->pFocusedWidget = nullptr;

    GUM_Widget_focusRelease_(pWidget);
    GBL_EMIT(pWidget, "onFocusLost", pDevice);
    GblBox_unref(GBL_BOX(pWidget));
}

void GUM_InputDevice_widgetDestroyed_(GUM_Widget* pWidget) {
    if (!pWidget)
        return;

    for (GUM_InputDevice* pDevice = pFirstDevice_; pDevice;
         pDevice = GUM_INPUTDEVICE_(pDevice)->pNext) {
        GUM_InputDevice_* pDevice_ = GUM_INPUTDEVICE_(pDevice);
        if (pDevice_->pFocusedWidget != pWidget)
            continue;

        pDevice_->pFocusedWidget = nullptr;
        GUM_Widget_focusRelease_(pWidget);
    }
}

static GBL_RESULT GUM_InputDevice_init_(GblInstance* pInstance) {
    GUM_InputDevice*  pSelf  = GUM_INPUTDEVICE(pInstance);
    GUM_InputDevice_* pSelf_ = GUM_INPUTDEVICE_(pSelf);

    pSelf_->pPrev             = nullptr;
    pSelf_->pNext             = pFirstDevice_;
    pSelf_->pDeviceName       = GblStringRef_create("UNKNOWN");
    pSelf_->pFocusedWidget    = nullptr;
    pSelf_->enabled           = true;
    pSelf_->focusRingEnabled  = true;
    pSelf_->baselinePending   = false;
    pSelf_->focusTransition   = false;

    if (pFirstDevice_)
        GUM_INPUTDEVICE_(pFirstDevice_)->pPrev = pSelf;
    pFirstDevice_ = pSelf;

    pSelf->buttons      = 0;
    pSelf->buttonsPrev  = 0;
    pSelf->highlight_r = 255;
    pSelf->highlight_g = 255;
    pSelf->highlight_b = 255;
    pSelf->highlight_a = 255;

    return pSelf_->pDeviceName ? GBL_RESULT_SUCCESS : GBL_RESULT_ERROR_MEM_ALLOC;
}

static GBL_RESULT GUM_InputDevice_GblBox_destructor_(GblBox* pBox) {
    GUM_InputDevice*  pSelf  = GUM_INPUTDEVICE(pBox);
    GUM_InputDevice_* pSelf_ = GUM_INPUTDEVICE_(pSelf);

    // The device is already finalizing, so focus loss must not retain it.
    pSelf_->focusTransition = true;
    GUM_InputDevice_focusRelease_(pSelf);

    if (pSelf_->pPrev)
        GUM_INPUTDEVICE_(pSelf_->pPrev)->pNext = pSelf_->pNext;
    else
        pFirstDevice_ = pSelf_->pNext;

    if (pSelf_->pNext)
        GUM_INPUTDEVICE_(pSelf_->pNext)->pPrev = pSelf_->pPrev;

    pSelf_->pPrev = nullptr;
    pSelf_->pNext = nullptr;

    GblStringRef_unref(pSelf_->pDeviceName);
    pSelf_->pDeviceName = nullptr;

    GblObjectClass* pObjClass = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GBL_OBJECT_TYPE));
    return pObjClass->base.pFnDestructor(pBox);
}

static GBL_RESULT GUM_InputDevice_GblObject_setProperty_(GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GUM_InputDevice* pSelf = GUM_INPUTDEVICE(pObject);
    switch (pProp->id) {
        case GUM_InputDevice_Property_Id_highlight_color: {
            const uint32_t color = GblVariant_uint32(pValue);
            pSelf->highlight_r = (color >> 24) & 0xFF;
            pSelf->highlight_g = (color >> 16) & 0xFF;
            pSelf->highlight_b = (color >> 8)  & 0xFF;
            pSelf->highlight_a =  color        & 0xFF;
            break;
        }
        case GUM_InputDevice_Property_Id_highlight_r:
            pSelf->highlight_r = GblVariant_uint8(pValue);
            break;
        case GUM_InputDevice_Property_Id_highlight_g:
            pSelf->highlight_g = GblVariant_uint8(pValue);
            break;
        case GUM_InputDevice_Property_Id_highlight_b:
            pSelf->highlight_b = GblVariant_uint8(pValue);
            break;
        case GUM_InputDevice_Property_Id_highlight_a:
            pSelf->highlight_a = GblVariant_uint8(pValue);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_InputDevice_GblObject_property_(const GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GUM_InputDevice* pSelf = GUM_INPUTDEVICE(pObject);
    switch (pProp->id) {
        case GUM_InputDevice_Property_Id_highlight_color:
            GblVariant_setUint32(pValue, (uint32_t)pSelf->highlight_r << 24 |
                                         (uint32_t)pSelf->highlight_g << 16 |
                                         (uint32_t)pSelf->highlight_b << 8  |
                                         (uint32_t)pSelf->highlight_a);
            break;
        case GUM_InputDevice_Property_Id_highlight_r:
            GblVariant_setUint8(pValue, pSelf->highlight_r);
            break;
        case GUM_InputDevice_Property_Id_highlight_g:
            GblVariant_setUint8(pValue, pSelf->highlight_g);
            break;
        case GUM_InputDevice_Property_Id_highlight_b:
            GblVariant_setUint8(pValue, pSelf->highlight_b);
            break;
        case GUM_InputDevice_Property_Id_highlight_a:
            GblVariant_setUint8(pValue, pSelf->highlight_a);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_InputDeviceClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_INPUTDEVICE_TYPE))
        GBL_PROPERTIES_REGISTER(GUM_InputDevice);

    GBL_BOX_CLASS(pClass)->pFnDestructor      = GUM_InputDevice_GblBox_destructor_;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GUM_InputDevice_GblObject_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty    = GUM_InputDevice_GblObject_property_;

    return GBL_RESULT_SUCCESS;
}

GblType GUM_InputDevice_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_InputDevice"),
                                GBL_OBJECT_TYPE,
                                &(static GblTypeInfo){ .classSize           = sizeof(GUM_InputDeviceClass),
                                                       .pFnClassInit        = GUM_InputDeviceClass_init_,
                                                       .instanceSize        = sizeof(GUM_InputDevice),
                                                       .instancePrivateSize = sizeof(GUM_InputDevice_),
                                                       .pFnInstanceInit     = GUM_InputDevice_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
