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

#include <gimbal/gimbal_containers.h>

#include "gumball_container_.h"
#include "gumball_root_.h"
#include "gumball_widget_.h"

#include <stdint.h>

#define GUM_ROOT_(self) (GBL_PRIVATE(GUM_Root, self))

typedef struct GUM_Root_ {
    GblLogger* pLogger;
    GblDoublyLinkedListNode drawOrder;
    GUM_Vector2 lastScreenSize;
    uint64_t    nextEnableOrder;
} GUM_Root_;

typedef struct GUM_RootSnapshot_ {
    GblArrayList widgets;
    GUM_Widget*  stack[GUM_SNAPSHOT_INLINE_CAPACITY];
} GUM_RootSnapshot_;

typedef struct GUM_RootChildSnapshot_ {
    GblArrayList objects;
    GblObject*   stack[GUM_SNAPSHOT_INLINE_CAPACITY];
} GUM_RootChildSnapshot_;

static int GUM_Root_widgetCmp_(const GUM_Widget* pA, const GUM_Widget* pB) {
    const GUM_Widget_* pA_ = GUM_WIDGET_(pA);
    const GUM_Widget_* pB_ = GUM_WIDGET_(pB);

    if (pA_->zIndex < pB_->zIndex) return -1;
    if (pA_->zIndex > pB_->zIndex) return 1;

    const size_t depthA = GblObject_depth(GBL_OBJECT(pA));
    const size_t depthB = GblObject_depth(GBL_OBJECT(pB));
    if (depthA < depthB) return -1;
    if (depthA > depthB) return 1;

    if (pA_->enableOrder < pB_->enableOrder) return -1;
    if (pA_->enableOrder > pB_->enableOrder) return 1;
    return 0;
}

static void GUM_Root_insertOrdered_(GUM_Root* pRoot, GUM_Widget* pWidget) {
    GblDoublyLinkedListNode* pHead = &GUM_ROOT_(pRoot)->drawOrder;
    GblDoublyLinkedListNode* pNode = pHead->pNext;

    for (; pNode != pHead; pNode = pNode->pNext) {
        GUM_Widget* pIt = GBL_DOUBLY_LINKED_LIST_ENTRY(pNode, GUM_Widget_, drawNode)->pWidget;
        if (GUM_Root_widgetCmp_(pIt, pWidget) > 0)
            break;
    }

    GblDoublyLinkedList_insertBefore(pNode, &GUM_WIDGET_(pWidget)->drawNode);
}

static int GUM_Root_drawNodeCmp_(const void* pA, const void* pB, void* pClosure) {
    GBL_UNUSED(pClosure);
    const GUM_Widget_* pA_ = GBL_DOUBLY_LINKED_LIST_ENTRY(pA, GUM_Widget_, drawNode);
    const GUM_Widget_* pB_ = GBL_DOUBLY_LINKED_LIST_ENTRY(pB, GUM_Widget_, drawNode);
    return GUM_Root_widgetCmp_(pA_->pWidget, pB_->pWidget);
}

static void GUM_Root_snapshotRelease_(GUM_RootSnapshot_* pSnapshot) {
    GUM_Widget* const* ppWidgets = GblArrayList_data(&pSnapshot->widgets);
    const size_t      count     = GblArrayList_size(&pSnapshot->widgets);

    for (size_t i = 0; i < count; ++i) {
        GUM_Widget* pWidget = ppWidgets[i];
        GblBox_unref(GBL_BOX(pWidget));
    }

    GblArrayList_destruct(&pSnapshot->widgets);
}

static GBL_RESULT GUM_Root_snapshot_(GUM_Root* pRoot, GUM_RootSnapshot_* pSnapshot) {
    GBL_RESULT result = GblArrayList_construct(&pSnapshot->widgets,
                                               sizeof(GUM_Widget*),
                                               0,
                                               nullptr,
                                               sizeof(*pSnapshot));
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    for (GblDoublyLinkedListNode* pNode = GUM_ROOT_(pRoot)->drawOrder.pNext;
         pNode != &GUM_ROOT_(pRoot)->drawOrder;
         pNode = pNode->pNext) {
        GUM_Widget* pIt       = GBL_DOUBLY_LINKED_LIST_ENTRY(pNode, GUM_Widget_, drawNode)->pWidget;
        GUM_Widget* pRetained = GUM_WIDGET(GblBox_ref(GBL_BOX(pIt)));
        result = GblArrayList_pushBack(&pSnapshot->widgets, &pRetained);

        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
            GblBox_unref(GBL_BOX(pRetained));
            GUM_Root_snapshotRelease_(pSnapshot);
            return result;
        }
    }

    return GBL_RESULT_SUCCESS;
}

static void GUM_Root_childSnapshotRelease_(GUM_RootChildSnapshot_* pSnapshot) {
    GblObject* const* ppObjects = GblArrayList_data(&pSnapshot->objects);
    const size_t     count     = GblArrayList_size(&pSnapshot->objects);

    for (size_t i = 0; i < count; ++i) {
        GblObject* pObject = ppObjects[i];
        GblBox_unref(GBL_BOX(pObject));
    }

    GblArrayList_destruct(&pSnapshot->objects);
}

static GBL_RESULT GUM_Root_childSnapshot_(GUM_Root* pRoot, GUM_RootChildSnapshot_* pSnapshot) {
    GBL_RESULT result = GblArrayList_construct(&pSnapshot->objects,
                                               sizeof(GblObject*),
                                               0,
                                               nullptr,
                                               sizeof(*pSnapshot));
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    GblObject_foreachChild(GBL_OBJECT(pRoot), pChild) {
        GblObject* pRetained = GBL_OBJECT(GblBox_ref(GBL_BOX(pChild)));
        result = GblArrayList_pushBack(&pSnapshot->objects, &pRetained);

        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
            GblBox_unref(GBL_BOX(pRetained));
            GUM_Root_childSnapshotRelease_(pSnapshot);
            return result;
        }
    }

    return GBL_RESULT_SUCCESS;
}

GUM_Root* GUM_Root_active_(void) {
    return GBL_AS(GUM_Root, GblModule_find("GUM_Root"));
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

    GblDoublyLinkedList_init(&pSelf_->drawOrder);
    pSelf_->nextEnableOrder = 0;

    pSelf_->pLogger = GblLogger_create(GBL_LOGGER_TYPE, sizeof(GblLogger), nullptr);
    if (!pSelf_->pLogger)
        return GBL_RESULT_ERROR_MEM_ALLOC;

    GBL_RESULT result = GblLogger_register(pSelf_->pLogger);
    if (!GBL_RESULT_SUCCESS(result)) {
        GblLogger_unref(pSelf_->pLogger);
        pSelf_->pLogger = nullptr;
        return result;
    }

    GUM_Backend_setLogger();

    result = GUM_InputSystem_init();
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        GUM_Backend_resetLogger();
        GblLogger_unregister(pSelf_->pLogger);
        GblLogger_unref(pSelf_->pLogger);
        pSelf_->pLogger = nullptr;
        return result;
    }

    result = GblModule_register(GBL_MODULE(pSelf));
    if (!GBL_RESULT_SUCCESS(result)) {
        GUM_InputSystem_deinit();
        GUM_Backend_resetLogger();
        GblLogger_unregister(pSelf_->pLogger);
        GblLogger_unref(pSelf_->pLogger);
        pSelf_->pLogger = nullptr;
        return result;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Root_GblBox_destructor_(GblBox* pBox) {
    GUM_Root*  pSelf  = GUM_ROOT(pBox);
    GUM_Root_* pSelf_ = GUM_ROOT_(pSelf);

    // Focus-loss callbacks still need framework services.
    GUM_InputSystem_deinit();
    GUM_Font_setDefault(nullptr);
    GUM_Manager_deinit();

    if (pSelf_->pLogger)
        GblLogger_unregister(pSelf_->pLogger);

    GUM_Backend_resetLogger();
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

void GUM_Root_drawEnable_(GUM_Widget* pWidget) {
    if (!pWidget)
        return;

    GUM_Root* pRoot = GUM_Root_active_();
    if (!pRoot ||
        GblObject_findAncestorByType(GBL_OBJECT(pWidget), GUM_ROOT_TYPE) != GBL_OBJECT(pRoot))
        return;

    GUM_Widget_* pWidget_ = GUM_WIDGET_(pWidget);
    if (pWidget_->drawMember)
        return;

    GUM_Root_* pRoot_ = GUM_ROOT_(pRoot);
    pWidget_->enableOrder = pRoot_->nextEnableOrder++;
    pWidget_->drawMember  = true;
    GUM_Root_insertOrdered_(pRoot, pWidget);
}

void GUM_Root_drawDisable_(GUM_Widget* pWidget) {
    GUM_Root* pRoot = GUM_Root_active_();
    if (!pRoot || !pWidget)
        return;

    GUM_Widget_* pWidget_ = GUM_WIDGET_(pWidget);
    if (!pWidget_->drawMember)
        return;

    GblDoublyLinkedList_remove(&pWidget_->drawNode);
    pWidget_->drawMember = false;
}

void GUM_Root_drawOrderChanged_(void) {
    GUM_Root* pRoot = GUM_Root_active_();
    if (pRoot)
        GblDoublyLinkedList_mergeSort(&GUM_ROOT_(pRoot)->drawOrder, GUM_Root_drawNodeCmp_, nullptr);
}

GBL_RESULT GUM_Root_foreachDrawable_(GUM_Root* pRoot,
                                     GUM_Root_WidgetIterFn_ pFnIter,
                                     void* pClosure) {
    if (!pRoot || !pFnIter)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    if (GblDoublyLinkedList_empty(&GUM_ROOT_(pRoot)->drawOrder))
        return GBL_RESULT_PARTIAL;

    GUM_RootSnapshot_ snapshot;
    GBL_RESULT        result = GUM_Root_snapshot_(pRoot, &snapshot);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    GUM_Widget* const* ppWidgets = GblArrayList_data(&snapshot.widgets);
    const size_t      count     = GblArrayList_size(&snapshot.widgets);
    for (size_t i = 0; i < count; ++i) {
        GUM_Widget* pWidget = ppWidgets[i];
        if (pFnIter(pWidget, pClosure))
            break;
    }

    GUM_Root_snapshotRelease_(&snapshot);
    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Root_draw_(GUM_Root* pRoot, GUM_Renderer* pRenderer) {
    if (!pRoot)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    if (GblDoublyLinkedList_empty(&GUM_ROOT_(pRoot)->drawOrder))
        return GBL_RESULT_PARTIAL;

    GUM_RootSnapshot_ snapshot;
    GBL_RESULT        result = GUM_Root_snapshot_(pRoot, &snapshot);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    GUM_Widget* const* ppWidgets = GblArrayList_data(&snapshot.widgets);

    GBL_RESULT   firstFailure = GBL_RESULT_SUCCESS;
    const size_t count        = GblArrayList_size(&snapshot.widgets);

    for (size_t i = 0; i < count; ++i) {
        GUM_Widget*      pWidget    = ppWidgets[i];
        const GBL_RESULT drawResult = GUM_WIDGET_CLASSOF(pWidget)->pFnDraw(pWidget, pRenderer);

        if (GBL_RESULT_SUCCESS(firstFailure) && !GBL_RESULT_SUCCESS(drawResult))
            firstFailure = drawResult;
    }

    GUM_Root_snapshotRelease_(&snapshot);
    return firstFailure;
}

static bool GUM_Root_scrollbarHit_(GUM_Widget* pWidget, GUM_Vector2 position) {
    GUM_Container* pContainer = GBL_AS(GUM_Container, pWidget);
    if (!pContainer)
        return false;

    GUM_Rectangle track;
    GUM_Rectangle thumb;
    if (!GUM_Container_scrollbarGeometry_(pContainer, &track, &thumb))
        return false;

    return position.x >= track.x && position.x < track.x + track.width &&
           position.y >= track.y && position.y < track.y + track.height;
}

static GUM_Widget* GUM_Root_pointerWidgetAt_(GUM_Root* pRoot,
                                             GUM_Vector2 position,
                                             bool activeOnly) {
    if (!pRoot)
        return nullptr;

    for (GblDoublyLinkedListNode* pNode = GUM_ROOT_(pRoot)->drawOrder.pPrev;
         pNode != &GUM_ROOT_(pRoot)->drawOrder;
         pNode = pNode->pPrev) {
        GUM_Widget* pWidget = GBL_DOUBLY_LINKED_LIST_ENTRY(pNode, GUM_Widget_, drawNode)->pWidget;
        if (!pWidget->isInteractive)
            continue;
        if (activeOnly && !GUM_Widget_isActive(pWidget) && !GUM_Root_scrollbarHit_(pWidget, position))
            continue;

        const GUM_Vector2   widgetPos  = GUM_get_absolute_position_(pWidget);
        const GUM_Vector2   widgetSize = { pWidget->w, pWidget->h };
        const GUM_Rectangle clip       = GUM_Widget_clipRect_(pWidget);
        const bool          inClip     = position.x >= clip.x && position.x < clip.x + clip.width &&
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

GUM_Widget* GUM_Root_pointerTargetAt_(GUM_Root* pRoot, GUM_Vector2 position) {
    return GUM_Root_pointerWidgetAt_(pRoot, position, true);
}

GUM_Widget* GUM_Root_pointerHoverAt_(GUM_Root* pRoot, GUM_Vector2 position) {
    return GUM_Root_pointerWidgetAt_(pRoot, position, false);
}

GBL_RESULT GUM_Root_update(GUM_Root* pRoot) {
    if (!pRoot)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (GUM_Root_active_() != pRoot)
        return GBL_RESULT_NOT_READY;

    GblBox_ref(GBL_BOX(pRoot));
    GUM_Root_* pSelf_       = GUM_ROOT_(pRoot);
    GBL_RESULT firstFailure = GBL_RESULT_SUCCESS;

    GUM_Backend_update();
    if (GUM_Root_active_() != pRoot)
        goto done;

    const GUM_Vector2 screenSize = GUM_Backend_screenSize();

    if (screenSize.x != pSelf_->lastScreenSize.x ||
        screenSize.y != pSelf_->lastScreenSize.y) {
        GUM_RootChildSnapshot_ snapshot;
        GBL_RESULT             result = GUM_Root_childSnapshot_(pRoot, &snapshot);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
            firstFailure = result;
            goto done;
        }

        GblObject* const* ppObjects = GblArrayList_data(&snapshot.objects);
        const size_t     count     = GblArrayList_size(&snapshot.objects);
        for (size_t i = 0; i < count; ++i) {
            if (GUM_Root_active_() != pRoot)
                break;

            GblObject*     pObject    = ppObjects[i];
            GUM_Container* pContainer = GBL_AS(GUM_Container, pObject);
            if (!pContainer)
                continue;

            result = GUM_CONTAINER_CLASSOF(pContainer)->pFnUpdateContent(pContainer);
            if (GBL_RESULT_SUCCESS(firstFailure) && !GBL_RESULT_SUCCESS(result))
                firstFailure = result;
        }

        const bool stillActive = GUM_Root_active_() == pRoot;
        GUM_Root_childSnapshotRelease_(&snapshot);
        if (!stillActive)
            goto done;
        if (!GBL_RESULT_SUCCESS(firstFailure))
            goto done;

        pSelf_->lastScreenSize = screenSize;
    }

    if (GUM_Root_active_() == pRoot)
        GUM_InputSystem_update();

done:
    GblBox_unref(GBL_BOX(pRoot));
    return firstFailure;
}
