#include <gumball/elements/gumball_tree.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/events/gumball_event_mouse.h>

#include <gimbal/gimbal_containers.h>

#include "../core/gumball_marshal_.h"
#include "gumball_scrollviewport_.h"
#include "gumball_widget_.h"

#include <stdint.h>

#define GUM_TREE_(self) (GBL_PRIVATE(GUM_Tree, self))

typedef struct GUM_Tree_ {
    GUM_IItemModel*     pModel;
    GUM_ModelIndex      selection;
    GUM_ScrollViewport_ viewport;
    GblArrayList        collapsed;
} GUM_Tree_;

static size_t GUM_Tree_collapsedIndex_(const GUM_Tree* pSelf, const void* pHandle) {
    const GblArrayList* pCollapsed = &GUM_TREE_(pSelf)->collapsed;
    const size_t count = GblArrayList_size(pCollapsed);
    for (size_t i = 0; i < count; ++i) {
        if (*(const void* const*)GblArrayList_at(pCollapsed, i) == pHandle)
            return i;
    }
    return SIZE_MAX;
}

static GblBool GUM_Tree_selectable_(const GUM_Tree* pSelf, GUM_ModelIndex index) {
    const GUM_IItemModel* pModel = GUM_TREE_(pSelf)->pModel;
    return pModel &&
           GUM_ModelIndex_belongs(index, pModel) &&
           index.column == 0 &&
           (GUM_IItemModel_flags(pModel, index) & GUM_ITEM_SELECTABLE);
}

static GblBool GUM_Tree_expanded_(const GUM_Tree* pSelf, GUM_ModelIndex index) {
    const GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    return pSelf_->pModel &&
           GUM_ModelIndex_belongs(index, pSelf_->pModel) &&
           index.column == 0 &&
           GUM_IItemModel_rowCount(pSelf_->pModel, index) > 0 &&
           GUM_Tree_collapsedIndex_(pSelf, index.pHandle) == SIZE_MAX;
}

static size_t GUM_Tree_visibleCount_(const GUM_Tree* pSelf, GUM_ModelIndex parent) {
    const GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    if (!pSelf_->pModel)
        return 0;

    size_t count = 0;
    const size_t rows = GUM_IItemModel_rowCount(pSelf_->pModel, parent);
    for (size_t row = 0; row < rows; ++row) {
        const GUM_ModelIndex index = GUM_IItemModel_index(pSelf_->pModel, row, 0, parent);
        if (!GUM_ModelIndex_valid(index))
            continue;

        ++count;
        if (GUM_Tree_expanded_(pSelf, index))
            count += GUM_Tree_visibleCount_(pSelf, index);
    }
    return count;
}

static GUM_ModelIndex GUM_Tree_visibleAt_(const GUM_Tree* pSelf,
                                          GUM_ModelIndex parent,
                                          size_t depth,
                                          size_t target,
                                          size_t* pCursor,
                                          size_t* pDepth) {
    const GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    if (!pSelf_->pModel)
        return GUM_MODEL_INDEX_INVALID;

    const size_t rows = GUM_IItemModel_rowCount(pSelf_->pModel, parent);
    for (size_t row = 0; row < rows; ++row) {
        const GUM_ModelIndex index = GUM_IItemModel_index(pSelf_->pModel, row, 0, parent);
        if (!GUM_ModelIndex_valid(index))
            continue;

        if ((*pCursor)++ == target) {
            if (pDepth)
                *pDepth = depth;
            return index;
        }

        if (GUM_Tree_expanded_(pSelf, index)) {
            const GUM_ModelIndex found = GUM_Tree_visibleAt_(pSelf,
                                                             index,
                                                             depth + 1,
                                                             target,
                                                             pCursor,
                                                             pDepth);
            if (GUM_ModelIndex_valid(found))
                return found;
        }
    }
    return GUM_MODEL_INDEX_INVALID;
}

static GblBool GUM_Tree_visibleRow_(const GUM_Tree* pSelf,
                                    GUM_ModelIndex parent,
                                    GUM_ModelIndex target,
                                    size_t* pCursor,
                                    size_t* pRow) {
    const GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    if (!pSelf_->pModel)
        return GBL_FALSE;

    const size_t rows = GUM_IItemModel_rowCount(pSelf_->pModel, parent);
    for (size_t row = 0; row < rows; ++row) {
        const GUM_ModelIndex index = GUM_IItemModel_index(pSelf_->pModel, row, 0, parent);
        if (!GUM_ModelIndex_valid(index))
            continue;

        if (GUM_ModelIndex_equal(index, target)) {
            *pRow = *pCursor;
            return GBL_TRUE;
        }

        ++*pCursor;
        if (GUM_Tree_expanded_(pSelf, index) &&
            GUM_Tree_visibleRow_(pSelf, index, target, pCursor, pRow))
            return GBL_TRUE;
    }
    return GBL_FALSE;
}

static GblBool GUM_Tree_selectVisible_(GUM_Tree* pSelf, size_t row, int direction) {
    const size_t count = GUM_Tree_visibleCount_(pSelf, GUM_MODEL_INDEX_INVALID);
    if (!count || row >= count || !direction)
        return GBL_FALSE;

    for (;;) {
        size_t cursor = 0;
        const GUM_ModelIndex index = GUM_Tree_visibleAt_(pSelf,
                                                         GUM_MODEL_INDEX_INVALID,
                                                         0,
                                                         row,
                                                         &cursor,
                                                         nullptr);
        if (GUM_Tree_selectable_(pSelf, index))
            return GBL_RESULT_SUCCESS(GUM_Tree_select(pSelf, index));

        if (direction < 0) {
            if (!row)
                break;
            --row;
        } else {
            if (++row >= count)
                break;
        }
    }
    return GBL_FALSE;
}

static GblBool GUM_Tree_selectParent_(GUM_Tree* pSelf, GUM_ModelIndex index) {
    GUM_IItemModel* pModel = GUM_TREE_(pSelf)->pModel;
    for (GUM_ModelIndex parent = GUM_IItemModel_parent(pModel, index);
         GUM_ModelIndex_valid(parent);
         parent = GUM_IItemModel_parent(pModel, parent)) {
        if (GUM_Tree_selectable_(pSelf, parent))
            return GBL_RESULT_SUCCESS(GUM_Tree_select(pSelf, parent));
    }
    return GBL_FALSE;
}

static void GUM_Tree_revealSelection_(GUM_Tree* pSelf) {
    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    if (!pSelf_->pModel || !GUM_ModelIndex_valid(pSelf_->selection))
        return;

    size_t cursor = 0;
    size_t row = 0;
    if (!GUM_Tree_visibleRow_(pSelf,
                              GUM_MODEL_INDEX_INVALID,
                              pSelf_->selection,
                              &cursor,
                              &row))
        return;

    const float top = (float)row * pSelf->rowHeight;
    const float bottom = top + pSelf->rowHeight;
    const float position = GUM_ScrollViewport_position_(&pSelf_->viewport, GUM_SCROLL_AXIS_Y);
    const float viewport = GBL_MAX(GUM_WIDGET(pSelf)->h, 0.0f);

    if (top < position)
        GUM_ScrollViewport_scrollTo_(&pSelf_->viewport, GUM_SCROLL_AXIS_Y, top);
    else if (bottom > position + viewport)
        GUM_ScrollViewport_scrollTo_(&pSelf_->viewport,
                                     GUM_SCROLL_AXIS_Y,
                                     bottom - viewport);
}

static void GUM_Tree_structureChanged_(GblInstance* pReceiver) {
    GUM_Tree* pSelf = GUM_TREE(pReceiver);
    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    const GblBool hadSelection = GUM_ModelIndex_valid(pSelf_->selection);

    pSelf_->selection = GUM_MODEL_INDEX_INVALID;
    GblArrayList_clear(&pSelf_->collapsed);
    GUM_ScrollViewport_reset_(&pSelf_->viewport);

    if (hadSelection)
        GBL_EMIT(pSelf, "selectionChanged", nullptr, (size_t)0);
}

static GBL_RESULT GUM_Tree_init_(GblInstance* pInstance) {
    GUM_Tree* pSelf = GUM_TREE(pInstance);
    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);

    pSelf->rowHeight   = 24.0f;
    pSelf->indentWidth = 18.0f;
    pSelf_->selection  = GUM_MODEL_INDEX_INVALID;
    GUM_ScrollViewport_init_(&pSelf_->viewport);

    GUM_Widget_initActive_(GUM_WIDGET(pSelf), true);
    GUM_WIDGET(pSelf)->a = 0;
    GUM_WIDGET(pSelf)->isSelectable = true;

    return GblArrayList_construct(&pSelf_->collapsed, sizeof(const void*));
}

static GBL_RESULT GUM_Tree_GblBox_destructor_(GblBox* pBox) {
    GUM_Tree* pSelf = GUM_TREE(pBox);
    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);

    if (pSelf_->pModel) {
        GblSignal_disconnect(GBL_INSTANCE(pSelf_->pModel),
                             "structureChanged",
                             GBL_INSTANCE(pSelf),
                             nullptr);
        GUM_IItemModel_unref(pSelf_->pModel);
        pSelf_->pModel = nullptr;
    }

    GblArrayList_destruct(&pSelf_->collapsed);
    return GBL_BOX_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE))->pFnDestructor(pBox);
}

static GBL_RESULT GUM_Tree_GblObject_setProperty_(GblObject* pObject,
                                                   const GblProperty* pProp,
                                                   GblVariant* pValue) {
    GUM_Tree* pSelf = GUM_TREE(pObject);
    switch (pProp->id) {
        case GUM_Tree_Property_Id_rowHeight:
            pSelf->rowHeight = GBL_MAX(GblVariant_float(pValue), 1.0f);
            return GBL_RESULT_SUCCESS;
        case GUM_Tree_Property_Id_indentWidth:
            pSelf->indentWidth = GBL_MAX(GblVariant_float(pValue), 0.0f);
            return GBL_RESULT_SUCCESS;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }
}

static GBL_RESULT GUM_Tree_GblObject_property_(const GblObject* pObject,
                                                const GblProperty* pProp,
                                                GblVariant* pValue) {
    const GUM_Tree* pSelf = GUM_TREE(pObject);
    switch (pProp->id) {
        case GUM_Tree_Property_Id_rowHeight:
            return GblVariant_setFloat(pValue, pSelf->rowHeight);
        case GUM_Tree_Property_Id_indentWidth:
            return GblVariant_setFloat(pValue, pSelf->indentWidth);
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }
}

static GBL_RESULT GUM_Tree_update_(GUM_Widget* pWidget) {
    GUM_Tree* pSelf = GUM_TREE(pWidget);
    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    const float contentHeight = (float)GUM_Tree_visibleCount_(pSelf,
                                                              GUM_MODEL_INDEX_INVALID)
                              * pSelf->rowHeight;

    GUM_ScrollViewport_setRange_(&pSelf_->viewport,
                                 GUM_SCROLL_AXIS_Y,
                                 GBL_MAX(contentHeight - pWidget->h, 0.0f));
    GUM_ScrollViewport_update_(&pSelf_->viewport, GUM_Backend_frametime());
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Tree_drawDisclosure_(GUM_Tree* pSelf,
                                           GUM_Renderer* pRenderer,
                                           GUM_ModelIndex index,
                                           float x,
                                           float y) {
    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    if (!GUM_IItemModel_rowCount(pSelf_->pModel, index))
        return GBL_RESULT_SUCCESS;

    const GUM_Color color = {
        GUM_WIDGET(pSelf)->font_r,
        GUM_WIDGET(pSelf)->font_g,
        GUM_WIDGET(pSelf)->font_b,
        GUM_WIDGET(pSelf)->font_a
    };
    const float size = GBL_MIN(9.0f, GBL_MAX(pSelf->rowHeight - 8.0f, 3.0f));
    const float top = y + (pSelf->rowHeight - size) * 0.5f;
    GBL_RESULT result = GUM_Backend_rectangleLinesDraw(pRenderer,
                                                       (GUM_Rectangle){ x, top, size, size },
                                                       0.0f,
                                                       1.0f,
                                                       color);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    result = GUM_Backend_rectangleDraw(pRenderer,
                                       (GUM_Rectangle){ x + 2.0f,
                                                        top + size * 0.5f,
                                                        GBL_MAX(size - 4.0f, 1.0f),
                                                        1.0f },
                                       0.0f,
                                       color);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result) || GUM_Tree_expanded_(pSelf, index))
        return result;

    return GUM_Backend_rectangleDraw(pRenderer,
                                     (GUM_Rectangle){ x + size * 0.5f,
                                                      top + 2.0f,
                                                      1.0f,
                                                      GBL_MAX(size - 4.0f, 1.0f) },
                                     0.0f,
                                     color);
}

static GBL_RESULT GUM_Tree_drawText_(GUM_Tree* pSelf,
                                     GUM_Renderer* pRenderer,
                                     GUM_ModelIndex index,
                                     GUM_Vector2 position) {
    GBL_VARIANT(value);
    GBL_RESULT result = GUM_IItemModel_displayData(GUM_TREE_(pSelf)->pModel, index, &value);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        GblVariant_destruct(&value);
        return result;
    }

    GBL_VARIANT(text);
    GblStringRef* pText = nullptr;
    const GblType valueType = GblVariant_typeOf(&value);

    if (GblType_check(valueType, GBL_STRING_TYPE)) {
        pText = GblVariant_string(&value);
    } else if (GblVariant_canConvert(valueType, GBL_STRING_TYPE)) {
        result = GblVariant_constructString(&text, "");
        if (GBL_RESULT_SUCCESS(result)) {
            result = GblVariant_convert(&value, &text);
            if (GBL_RESULT_SUCCESS(result))
                pText = GblVariant_string(&text);
        }
    } else {
        pText = (GblStringRef*)GblVariant_typeName(&value);
    }

    if (GBL_RESULT_SUCCESS(result) && pText) {
        GUM_Widget* pWidget = GUM_WIDGET(pSelf);
        result = GUM_Backend_Font_draw(pRenderer,
                                       GUM_Widget_font(pWidget),
                                       pText,
                                       position,
                                       (GUM_Color){ pWidget->font_r,
                                                    pWidget->font_g,
                                                    pWidget->font_b,
                                                    pWidget->font_a },
                                       pWidget->font_size,
                                       1.2f);
    }

    GblVariant_destruct(&text);
    GblVariant_destruct(&value);
    return result;
}

static GBL_RESULT GUM_Tree_drawRows_(GUM_Tree* pSelf,
                                     GUM_Renderer* pRenderer,
                                     GUM_ModelIndex parent,
                                     size_t depth,
                                     size_t* pRow,
                                     GUM_Rectangle rec) {
    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    const float scroll = GUM_ScrollViewport_position_(&pSelf_->viewport, GUM_SCROLL_AXIS_Y);
    const size_t rows = GUM_IItemModel_rowCount(pSelf_->pModel, parent);

    for (size_t row = 0; row < rows; ++row) {
        const GUM_ModelIndex index = GUM_IItemModel_index(pSelf_->pModel, row, 0, parent);
        if (!GUM_ModelIndex_valid(index))
            continue;

        const float y = rec.y + (float)(*pRow) * pSelf->rowHeight - scroll;
        ++*pRow;

        if (y + pSelf->rowHeight > rec.y && y < rec.y + rec.height) {
            if (GUM_ModelIndex_equal(index, pSelf_->selection)) {
                const GBL_RESULT result = GUM_Backend_rectangleDraw(
                    pRenderer,
                    (GUM_Rectangle){ rec.x, y, rec.width, pSelf->rowHeight },
                    0.0f,
                    (GUM_Color){ GUM_WIDGET(pSelf)->font_r,
                                 GUM_WIDGET(pSelf)->font_g,
                                 GUM_WIDGET(pSelf)->font_b,
                                 48 });
                if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
                    return result;
            }

            const float nodeX = rec.x + 4.0f + (float)depth * pSelf->indentWidth;
            GBL_RESULT result = GUM_Tree_drawDisclosure_(pSelf, pRenderer, index, nodeX, y);
            if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
                return result;

            result = GUM_Tree_drawText_(pSelf,
                                        pRenderer,
                                        index,
                                        (GUM_Vector2){ nodeX + 13.0f,
                                                       y + (pSelf->rowHeight
                                                          - GUM_WIDGET(pSelf)->font_size) * 0.5f });
            if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
                return result;
        }

        if (GUM_Tree_expanded_(pSelf, index)) {
            const GBL_RESULT result = GUM_Tree_drawRows_(pSelf,
                                                         pRenderer,
                                                         index,
                                                         depth + 1,
                                                         pRow,
                                                         rec);
            if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
                return result;
        }
    }
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Tree_draw_(GUM_Widget* pWidget, GUM_Renderer* pRenderer) {
    GBL_RESULT result = GUM_WIDGET_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE))->pFnDraw(pWidget,
                                                                                            pRenderer);
    GUM_Tree* pSelf = GUM_TREE(pWidget);
    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result) || !pSelf_->pModel)
        return result;

    const GUM_Vector2 position = GUM_get_absolute_position_(pWidget);
    const GUM_Rectangle rec = { position.x, position.y, pWidget->w, pWidget->h };
    const GUM_Rectangle clip = GUM_Rectangle_intersect(GUM_Widget_clipRect_(pWidget), rec);
    if (clip.width <= 0.0f || clip.height <= 0.0f)
        return GBL_RESULT_SUCCESS;

    result = GUM_Backend_beginScissor(pRenderer, clip);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    size_t row = 0;
    result = GUM_Tree_drawRows_(pSelf,
                                pRenderer,
                                GUM_MODEL_INDEX_INVALID,
                                0,
                                &row,
                                rec);

    const GBL_RESULT endResult = GUM_Backend_endScissor(pRenderer);
    return GBL_RESULT_SUCCESS(result) ? endResult : result;
}

static GBL_RESULT GUM_Tree_pointerPress_(GUM_Tree* pSelf, GUM_Event_Mouse* pEvent) {
    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    if (!pSelf_->pModel || GUM_EVENT_INPUT(pEvent)->state != GUM_INPUTSTATE_PRESS ||
        GUM_EVENT_INPUT(pEvent)->button != GUM_MOUSE_BUTTON_LEFT)
        return GBL_RESULT_PARTIAL;

    const GUM_Vector2 position = GUM_get_absolute_position_(GUM_WIDGET(pSelf));
    const GUM_Vector2 pointer = GUM_EVENT_POINTER(pEvent)->position;
    const float localY = pointer.y - position.y
                       + GUM_ScrollViewport_position_(&pSelf_->viewport, GUM_SCROLL_AXIS_Y);
    if (localY < 0.0f || pSelf->rowHeight <= 0.0f)
        return GBL_RESULT_PARTIAL;

    const size_t row = (size_t)(localY / pSelf->rowHeight);
    if (row >= GUM_Tree_visibleCount_(pSelf, GUM_MODEL_INDEX_INVALID))
        return GBL_RESULT_PARTIAL;

    size_t cursor = 0;
    size_t depth = 0;
    const GUM_ModelIndex index = GUM_Tree_visibleAt_(pSelf,
                                                     GUM_MODEL_INDEX_INVALID,
                                                     0,
                                                     row,
                                                     &cursor,
                                                     &depth);
    if (!GUM_ModelIndex_valid(index))
        return GBL_RESULT_PARTIAL;

    const float nodeX = position.x + 4.0f + (float)depth * pSelf->indentWidth;
    const GblBool hasChildren = GUM_IItemModel_rowCount(pSelf_->pModel, index) > 0;
    if (hasChildren && pointer.x >= nodeX && pointer.x < nodeX + 13.0f) {
        const GBL_RESULT result = GUM_Tree_setExpanded(pSelf,
                                                       index,
                                                       !GUM_Tree_expanded_(pSelf, index));
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
            return result;
    }

    return GUM_Tree_selectable_(pSelf, index) ? GUM_Tree_select(pSelf, index)
                                               : GBL_RESULT_PARTIAL;
}

static GBL_RESULT GUM_Tree_inputEvent_(GUM_Widget* pWidget, GUM_Event_Input* pEvent) {
    GUM_Tree* pSelf = GUM_TREE(pWidget);
    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    GUM_Event_Mouse* pMouseEvent = GBL_AS(GUM_Event_Mouse, pEvent);

    if (pMouseEvent && pSelf_->pModel &&
        GUM_ScrollViewport_range_(&pSelf_->viewport, GUM_SCROLL_AXIS_Y) > 0.0f) {
        const float delta = pMouseEvent->wheel.y * -70.0f;
        if (delta) {
            GUM_ScrollViewport_scrollBy_(&pSelf_->viewport, GUM_SCROLL_AXIS_Y, delta);
            GblEvent_accept(GBL_EVENT(pEvent));
            return GBL_RESULT_SUCCESS;
        }
    }

    if (pMouseEvent) {
        const GBL_RESULT result = GUM_Tree_pointerPress_(pSelf, pMouseEvent);
        if (result != GBL_RESULT_PARTIAL && !GBL_RESULT_SUCCESS(result))
            return result;
    }

    return GUM_WIDGET_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE))->pFnInputEvent(pWidget,
                                                                                     pEvent);
}

static GblBool GUM_Tree_navigate_(GUM_Widget* pWidget, GUM_InputAction action) {
    GUM_Tree* pSelf = GUM_TREE(pWidget);
    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    if (!pSelf_->pModel)
        return GBL_FALSE;

    const size_t visibleCount = GUM_Tree_visibleCount_(pSelf, GUM_MODEL_INDEX_INVALID);
    if (!visibleCount)
        return GBL_FALSE;

    size_t cursor = 0;
    size_t currentRow = 0;
    const GblBool hasSelection = GUM_ModelIndex_valid(pSelf_->selection) &&
        GUM_Tree_visibleRow_(pSelf,
                             GUM_MODEL_INDEX_INVALID,
                             pSelf_->selection,
                             &cursor,
                             &currentRow);

    GblBool handled = GBL_FALSE;
    switch (action) {
        case GUM_INPUTACTION_MOVE_UP:
            handled = hasSelection && currentRow > 0
                    ? GUM_Tree_selectVisible_(pSelf, currentRow - 1, -1)
                    : !hasSelection && GUM_Tree_selectVisible_(pSelf, 0, 1);
            break;

        case GUM_INPUTACTION_MOVE_DOWN:
            handled = hasSelection && currentRow + 1 < visibleCount
                    ? GUM_Tree_selectVisible_(pSelf, currentRow + 1, 1)
                    : !hasSelection && GUM_Tree_selectVisible_(pSelf, 0, 1);
            break;

        case GUM_INPUTACTION_MOVE_RIGHT:
            if (hasSelection && GUM_IItemModel_rowCount(pSelf_->pModel, pSelf_->selection) > 0) {
                if (!GUM_Tree_expanded_(pSelf, pSelf_->selection))
                    handled = GBL_RESULT_SUCCESS(GUM_Tree_setExpanded(pSelf,
                                                                       pSelf_->selection,
                                                                       GBL_TRUE));
            }
            break;

        case GUM_INPUTACTION_MOVE_LEFT:
            if (hasSelection) {
                if (GUM_Tree_expanded_(pSelf, pSelf_->selection))
                    handled = GBL_RESULT_SUCCESS(GUM_Tree_setExpanded(pSelf,
                                                                       pSelf_->selection,
                                                                       GBL_FALSE));
                else
                    handled = GUM_Tree_selectParent_(pSelf, pSelf_->selection);
            }
            break;

        default:
            break;
    }

    if (handled)
        GUM_Tree_revealSelection_(pSelf);
    return handled;
}

static GBL_RESULT GUM_TreeClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_TREE_TYPE)) {
        GBL_PROPERTIES_REGISTER(GUM_Tree);

        GblSignal_install(GUM_TREE_TYPE, "selectionChanged",
                          GUM_Marshal_CClosure_VOID__INSTANCE_POINTER_SIZE_, 2,
                          GBL_POINTER_TYPE, GBL_SIZE_TYPE);
    }

    GBL_BOX_CLASS(pClass)->pFnDestructor      = GUM_Tree_GblBox_destructor_;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GUM_Tree_GblObject_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty    = GUM_Tree_GblObject_property_;
    GUM_WIDGET_CLASS(pClass)->pFnUpdate      = GUM_Tree_update_;
    GUM_WIDGET_CLASS(pClass)->pFnDraw        = GUM_Tree_draw_;
    GUM_WIDGET_CLASS(pClass)->pFnInputEvent  = GUM_Tree_inputEvent_;
    GUM_WIDGET_CLASS(pClass)->pFnNavigate    = GUM_Tree_navigate_;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_TreeClass_final_(GblClass* pClass, const void* pClassData) {
    GBL_UNUSED(pClassData);

    if (!GblType_classRefCount(GUM_TREE_TYPE)) {
        GblProperty_uninstallAll(GUM_TREE_TYPE);
        GblSignal_uninstall(GUM_TREE_TYPE, "selectionChanged");
    }

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GUM_IItemModel* GUM_Tree_model(const GUM_Tree* pSelf) {
    return pSelf ? GUM_TREE_(pSelf)->pModel : nullptr;
}

GBL_EXPORT GBL_RESULT GUM_Tree_setModel(GUM_Tree* pSelf, GUM_IItemModel* pModel) {
    if (!pSelf)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    if (pSelf_->pModel == pModel)
        return GBL_RESULT_SUCCESS;

    if (pModel) {
        GUM_IItemModel_ref(pModel);
        const GBL_RESULT result = GblSignal_connect(GBL_INSTANCE(pModel),
                                                    "structureChanged",
                                                    GBL_INSTANCE(pSelf),
                                                    (GblFnPtr)GUM_Tree_structureChanged_,
                                                    nullptr);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
            GUM_IItemModel_unref(pModel);
            return result;
        }
    }

    if (pSelf_->pModel) {
        GblSignal_disconnect(GBL_INSTANCE(pSelf_->pModel),
                             "structureChanged",
                             GBL_INSTANCE(pSelf),
                             nullptr);
        GUM_IItemModel_unref(pSelf_->pModel);
    }

    const GblBool hadSelection = GUM_ModelIndex_valid(pSelf_->selection);
    pSelf_->pModel = pModel;
    pSelf_->selection = GUM_MODEL_INDEX_INVALID;
    GblArrayList_clear(&pSelf_->collapsed);
    GUM_ScrollViewport_reset_(&pSelf_->viewport);

    return hadSelection ? GBL_EMIT(pSelf, "selectionChanged", nullptr, (size_t)0)
                        : GBL_RESULT_SUCCESS;
}

GBL_EXPORT GUM_ModelIndex GUM_Tree_selection(const GUM_Tree* pSelf) {
    return pSelf ? GUM_TREE_(pSelf)->selection : GUM_MODEL_INDEX_INVALID;
}

GBL_EXPORT GBL_RESULT GUM_Tree_select(GUM_Tree* pSelf, GUM_ModelIndex index) {
    if (!pSelf)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    if (GUM_ModelIndex_valid(index)) {
        if (!GUM_Tree_selectable_(pSelf, index))
            return GBL_RESULT_ERROR_INVALID_ARG;
    }

    if (GUM_ModelIndex_equal(pSelf_->selection, index))
        return GBL_RESULT_SUCCESS;

    pSelf_->selection = index;
    return GBL_EMIT(pSelf,
                    "selectionChanged",
                    (void*)index.pHandle,
                    GUM_ModelIndex_valid(index) ? index.column : (size_t)0);
}

GBL_EXPORT GblBool GUM_Tree_expanded(const GUM_Tree* pSelf, GUM_ModelIndex index) {
    return pSelf ? GUM_Tree_expanded_(pSelf, index) : GBL_FALSE;
}

GBL_EXPORT GBL_RESULT GUM_Tree_setExpanded(GUM_Tree* pSelf,
                                           GUM_ModelIndex index,
                                           GblBool expanded) {
    if (!pSelf || !GUM_ModelIndex_valid(index))
        return GBL_RESULT_ERROR_INVALID_ARG;

    GUM_Tree_* pSelf_ = GUM_TREE_(pSelf);
    if (!pSelf_->pModel || !GUM_ModelIndex_belongs(index, pSelf_->pModel) || index.column != 0)
        return GBL_RESULT_ERROR_INVALID_ARG;
    if (!GUM_IItemModel_rowCount(pSelf_->pModel, index))
        return GBL_RESULT_SUCCESS;

    const size_t position = GUM_Tree_collapsedIndex_(pSelf, index.pHandle);
    if (expanded) {
        return position != SIZE_MAX ? GblArrayList_erase(&pSelf_->collapsed, position, 1)
                                    : GBL_RESULT_SUCCESS;
    }

    if (position == SIZE_MAX) {
        const void* pHandle = index.pHandle;
        return GblArrayList_pushBack(&pSelf_->collapsed, &pHandle);
    }
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT float GUM_Tree_scrollPosition(const GUM_Tree* pSelf) {
    return pSelf ? GUM_ScrollViewport_position_(&GUM_TREE_(pSelf)->viewport, GUM_SCROLL_AXIS_Y)
                 : 0.0f;
}

GBL_EXPORT float GUM_Tree_scrollRange(const GUM_Tree* pSelf) {
    return pSelf ? GUM_ScrollViewport_range_(&GUM_TREE_(pSelf)->viewport, GUM_SCROLL_AXIS_Y)
                 : 0.0f;
}

GBL_EXPORT void GUM_Tree_scrollTo(GUM_Tree* pSelf, float position) {
    if (pSelf)
        GUM_ScrollViewport_scrollTo_(&GUM_TREE_(pSelf)->viewport, GUM_SCROLL_AXIS_Y, position);
}

GBL_EXPORT void GUM_Tree_scrollBy(GUM_Tree* pSelf, float delta) {
    if (pSelf)
        GUM_ScrollViewport_scrollBy_(&GUM_TREE_(pSelf)->viewport, GUM_SCROLL_AXIS_Y, delta);
}

GBL_EXPORT GblType GUM_Tree_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if GBL_UNLIKELY (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Tree"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){
                                    .classSize           = sizeof(GUM_TreeClass),
                                    .instanceSize        = sizeof(GUM_Tree),
                                    .instancePrivateSize = sizeof(GUM_Tree_),
                                    .pFnClassInit        = GUM_TreeClass_init_,
                                    .pFnInstanceInit     = GUM_Tree_init_,
                                    .pFnClassFinal       = GUM_TreeClass_final_
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}
