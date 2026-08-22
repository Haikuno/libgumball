#include <gumball/elements/gumball_widget.h>
#include <gumball/elements/gumball_container.h>
#include <gumball/elements/gumball_root.h>
#include <gumball/elements/gumball_common.h>
#include <gumball/types/gumball_font.h>
#include <gumball/types/gumball_renderer.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_inputsystem.h>
#include <gumball/core/gumball_manager.h>

#include <gimbal/gimbal_algorithms.h>

#include "gumball_root_.h"

#include <stdint.h>

#define GUM_ROOT_(self) (GBL_PRIVATE(GUM_Root, self))

typedef struct GUM_DrawEntry_ {
    GUM_Widget* pWidget;
    uint64_t    enableOrder;
} GUM_DrawEntry_;

typedef struct GUM_Root_ {
    GblLogger*   pLogger;
    GblArrayList drawQueue;
    GUM_Vector2  lastScreenSize;
    uint64_t     nextEnableOrder;
} GUM_Root_;

static int GUM_Root_drawEntryCmp_(const void* pA, const void* pB) {
    const GUM_DrawEntry_* pEntryA = pA;
    const GUM_DrawEntry_* pEntryB = pB;
    const uint8_t zA = pEntryA->pWidget->z_index;
    const uint8_t zB = pEntryB->pWidget->z_index;

    if (zA < zB) return -1;
    if (zA > zB) return 1;
    if (pEntryA->enableOrder < pEntryB->enableOrder) return -1;
    if (pEntryA->enableOrder > pEntryB->enableOrder) return 1;
    return 0;
}

static void GUM_Root_drawQueueSort_(GUM_Root* pRoot) {
    GUM_Root_* pSelf_ = GUM_ROOT_(pRoot);
    gblSortInsertion(GblArrayList_data(&pSelf_->drawQueue),
                     GblArrayList_size(&pSelf_->drawQueue),
                     sizeof(GUM_DrawEntry_),
                     GUM_Root_drawEntryCmp_);
}

static void GUM_Root_rebaseEnableOrder_(GUM_Root* pRoot) {
    GUM_Root_* pSelf_ = GUM_ROOT_(pRoot);
    GUM_Root_drawQueueSort_(pRoot);

    const size_t count = GblArrayList_size(&pSelf_->drawQueue);
    for (size_t i = 0; i < count; ++i) {
        GUM_DrawEntry_* pEntry = GblArrayList_at(&pSelf_->drawQueue, i);
        pEntry->enableOrder = i;
    }

    pSelf_->nextEnableOrder = count;
}

GUM_Root* GUM_Root_active_(void) {
    GblModule* pModule = GblModule_find("GUM_Root");
    return pModule && GblType_check(GBL_TYPEOF(pModule), GUM_ROOT_TYPE) ?
           GUM_ROOT(pModule) : nullptr;
}

GBL_EXPORT GUM_Root* GUM_Root_create(void) {
    if GBL_UNLIKELY (GblModule_find("GUM_Root")) {
        GUM_LOG_ERROR("Only one GUM_Root may be active at a time.");
        return nullptr;
    }

    return GUM_ROOT(GBL_NEW(GUM_Root));
}

static GBL_RESULT GUM_Root_init_(GblInstance* pInstance) {
    GUM_Root*  pSelf  = GUM_ROOT(pInstance);
    GUM_Root_* pSelf_ = GUM_ROOT_(pSelf);

    GBL_RESULT result = GblArrayList_construct(&pSelf_->drawQueue, sizeof(GUM_DrawEntry_));
    if (!GBL_RESULT_SUCCESS(result))
        return result;

    pSelf_->pLogger = GblLogger_create(GBL_LOGGER_TYPE, sizeof(GblLogger), nullptr);
    if (!pSelf_->pLogger) {
        GblArrayList_destruct(&pSelf_->drawQueue);
        return GBL_RESULT_ERROR_MEM_ALLOC;
    }

    result = GblLogger_register(pSelf_->pLogger);
    if (!GBL_RESULT_SUCCESS(result)) {
        GblLogger_unref(pSelf_->pLogger);
        pSelf_->pLogger = nullptr;
        GblArrayList_destruct(&pSelf_->drawQueue);
        return result;
    }

    GUM_Backend_setLogger();
    GUM_InputSystem_init();

    result = GblModule_register(GBL_MODULE(pSelf));
    if (!GBL_RESULT_SUCCESS(result)) {
        GUM_InputSystem_deinit();
        GUM_Backend_resetLogger();
        GblLogger_unregister(pSelf_->pLogger);
        GblLogger_unref(pSelf_->pLogger);
        pSelf_->pLogger = nullptr;
        GblArrayList_destruct(&pSelf_->drawQueue);
        return result;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Root_GblBox_destructor_(GblBox* pBox) {
    GUM_Root*  pSelf  = GUM_ROOT(pBox);
    GUM_Root_* pSelf_ = GUM_ROOT_(pSelf);

    GblArrayList_destruct(&pSelf_->drawQueue);
    GUM_Font_setDefault(nullptr);
    GUM_Manager_deinit();

    if (pSelf_->pLogger)
        GblLogger_unregister(pSelf_->pLogger);

    GUM_Backend_resetLogger();
    GUM_InputSystem_deinit();
    GUM_Backend_deinit();

    if (pSelf_->pLogger) {
        GblLogger_unref(pSelf_->pLogger);
        pSelf_->pLogger = nullptr;
    }

    GblModuleClass* pModuleClass = GBL_MODULE_CLASS(GblClass_weakRefDefault(GBL_MODULE_TYPE));
    return GBL_BOX_CLASS(pModuleClass)->pFnDestructor(pBox);
}

static GBL_RESULT GUM_RootClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GBL_BOX_CLASS(pClass)->pFnDestructor = GUM_Root_GblBox_destructor_;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_Root_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Root"),
                                GBL_MODULE_TYPE,
                                &(static GblTypeInfo){ .classSize           = sizeof(GUM_RootClass),
                                                       .instanceSize        = sizeof(GUM_Root),
                                                       .instancePrivateSize = sizeof(GUM_Root_),
                                                       .pFnClassInit        = GUM_RootClass_init_,
                                                       .pFnInstanceInit     = GUM_Root_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

GBL_RESULT GUM_Root_drawEnable_(GUM_Widget* pWidget) {
    if (!pWidget)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_Root* pRoot = GUM_Root_active_();
    if (!pRoot)
        return GBL_RESULT_NOT_FOUND;

    GblObject* pSceneRoot = GblObject_findAncestorByType(GBL_OBJECT(pWidget), GUM_ROOT_TYPE);
    if (pSceneRoot != GBL_OBJECT(pRoot))
        return GBL_RESULT_NOT_FOUND;

    GUM_Root_* pSelf_ = GUM_ROOT_(pRoot);
    const size_t count = GblArrayList_size(&pSelf_->drawQueue);

    for (size_t i = 0; i < count; ++i) {
        const GUM_DrawEntry_* pEntry = GblArrayList_at(&pSelf_->drawQueue, i);
        if (pEntry->pWidget == pWidget)
            return GBL_RESULT_SUCCESS;
    }

    if (pSelf_->nextEnableOrder == UINT64_MAX)
        GUM_Root_rebaseEnableOrder_(pRoot);

    const GUM_DrawEntry_ entry = {
        .pWidget     = pWidget,
        .enableOrder = pSelf_->nextEnableOrder++
    };

    const GBL_RESULT result = GblArrayList_pushBack(&pSelf_->drawQueue, &entry);
    if (GBL_RESULT_SUCCESS(result))
        GUM_Root_drawQueueSort_(pRoot);

    return result;
}

void GUM_Root_drawDisable_(GUM_Widget* pWidget) {
    GUM_Root* pRoot = GUM_Root_active_();
    if (!pRoot || !pWidget)
        return;

    GUM_Root_* pSelf_ = GUM_ROOT_(pRoot);
    const size_t count = GblArrayList_size(&pSelf_->drawQueue);

    for (size_t i = 0; i < count; ++i) {
        const GUM_DrawEntry_* pEntry = GblArrayList_at(&pSelf_->drawQueue, i);
        if (pEntry->pWidget == pWidget) {
            GblArrayList_erase(&pSelf_->drawQueue, i, 1);
            return;
        }
    }
}

void GUM_Root_drawOrderChanged_(GUM_Widget* pWidget) {
    GUM_Root* pRoot = GUM_Root_active_();
    if (!pRoot || !pWidget)
        return;

    GUM_Root_* pSelf_ = GUM_ROOT_(pRoot);
    const size_t count = GblArrayList_size(&pSelf_->drawQueue);

    for (size_t i = 0; i < count; ++i) {
        const GUM_DrawEntry_* pEntry = GblArrayList_at(&pSelf_->drawQueue, i);
        if (pEntry->pWidget == pWidget) {
            GUM_Root_drawQueueSort_(pRoot);
            return;
        }
    }
}

void GUM_Root_foreachDrawable_(GUM_Root* pRoot,
                               GUM_Root_WidgetIterFn_ pFnIter,
                               void* pClosure) {
    if (!pRoot || !pFnIter)
        return;

    GUM_Root_* pSelf_ = GUM_ROOT_(pRoot);
    const size_t count = GblArrayList_size(&pSelf_->drawQueue);

    for (size_t i = 0; i < count; ++i) {
        const GUM_DrawEntry_* pEntry = GblArrayList_at(&pSelf_->drawQueue, i);
        if (pFnIter(pEntry->pWidget, pClosure))
            return;
    }
}

GBL_RESULT GUM_Root_draw_(GUM_Root* pRoot, GUM_Renderer* pRenderer) {
    if (!pRoot)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_Root_* pSelf_ = GUM_ROOT_(pRoot);
    const size_t count = GblArrayList_size(&pSelf_->drawQueue);
    if (!count)
        return GBL_RESULT_PARTIAL;

    for (size_t i = 0; i < count; ++i) {
        const GUM_DrawEntry_* pEntry = GblArrayList_at(&pSelf_->drawQueue, i);
        GUM_Widget* pWidget = pEntry->pWidget;
        GUM_WIDGET_CLASSOF(pWidget)->pFnDraw(pWidget, pRenderer);
    }

    return GBL_RESULT_SUCCESS;
}

GUM_Widget* GUM_Root_pointerTargetAt_(GUM_Root* pRoot, GUM_Vector2 position) {
    if (!pRoot)
        return nullptr;

    GUM_Root_* pSelf_ = GUM_ROOT_(pRoot);

    for (size_t i = GblArrayList_size(&pSelf_->drawQueue); i-- > 0;) {
        const GUM_DrawEntry_* pEntry = GblArrayList_at(&pSelf_->drawQueue, i);
        GUM_Widget* pWidget = pEntry->pWidget;

        if (!pWidget->isInteractive || !pWidget->isActive)
            continue;

        const GUM_Vector2 widgetPos  = GUM_get_absolute_position_(pWidget);
        const GUM_Vector2 widgetSize = { pWidget->w, pWidget->h };
        const GUM_Rectangle clip = pWidget->clipRect;
        const bool inClip = position.x >= clip.x && position.x < clip.x + clip.width &&
                            position.y >= clip.y && position.y < clip.y + clip.height;

        if (inClip &&
            position.x >= widgetPos.x &&
            position.x <  widgetPos.x + widgetSize.x &&
            position.y >= widgetPos.y &&
            position.y <  widgetPos.y + widgetSize.y) {
            return pWidget;
        }
    }

    return nullptr;
}

void GUM_Root_update(GUM_Root* pRoot) {
    GUM_Root_* pSelf_ = GUM_ROOT_(pRoot);

    GUM_Backend_update();

    const GUM_Vector2 screenSize = GUM_Backend_screenSize();

    if (screenSize.x != pSelf_->lastScreenSize.x ||
        screenSize.y != pSelf_->lastScreenSize.y) {
        GblObject_foreachChild(GBL_OBJECT(pRoot), pContainer, GUM_Container*) {
            if (GblType_check(GBL_TYPEOF(pContainer), GUM_CONTAINER_TYPE))
                GUM_CONTAINER_CLASSOF(pContainer)->pFnUpdateContent(pContainer);
        }
    }

    pSelf_->lastScreenSize = screenSize;

    GUM_InputSystem_update();
}
