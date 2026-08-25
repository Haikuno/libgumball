#include <gumball/elements/gumball_table.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/events/gumball_event_mouse.h>

#include "../core/gumball_marshal_.h"
#include "gumball_scrollviewport_.h"
#include "gumball_widget_.h"

#define GUM_TABLE_(self) (GBL_PRIVATE(GUM_Table, self))

typedef struct GUM_Table_ {
    GUM_IItemModel*     pModel;
    GUM_ModelIndex      selection;
    GUM_ScrollViewport_ viewport;
} GUM_Table_;

static void GUM_Table_structureChanged_(GblInstance* pReceiver) {
    GUM_Table* pSelf = GUM_TABLE(pReceiver);
    GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    const GblBool hadSelection = GUM_ModelIndex_valid(pSelf_->selection);

    pSelf_->selection = GUM_MODEL_INDEX_INVALID;
    GUM_ScrollViewport_reset_(&pSelf_->viewport);

    if (hadSelection)
        GBL_EMIT(pSelf, "selectionChanged", nullptr, (size_t)0);
}

static GBL_RESULT GUM_Table_init_(GblInstance* pInstance) {
    GUM_Table* pSelf = GUM_TABLE(pInstance);
    GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);

    pSelf->rowHeight  = 24.0f;
    pSelf_->selection = GUM_MODEL_INDEX_INVALID;
    GUM_ScrollViewport_init_(&pSelf_->viewport);

    GUM_WIDGET(pSelf)->a = 0;
    GUM_WIDGET(pSelf)->isSelectable = true;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Table_GblBox_destructor_(GblBox* pBox) {
    GUM_Table* pSelf = GUM_TABLE(pBox);
    GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);

    if (pSelf_->pModel) {
        GblSignal_disconnect(GBL_INSTANCE(pSelf_->pModel),
                             "structureChanged",
                             GBL_INSTANCE(pSelf),
                             nullptr);
        GUM_IItemModel_unref(pSelf_->pModel);
        pSelf_->pModel = nullptr;
    }

    return GBL_BOX_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE))->pFnDestructor(pBox);
}

static GBL_RESULT GUM_Table_GblObject_setProperty_(GblObject* pObject,
                                                    const GblProperty* pProp,
                                                    GblVariant* pValue) {
    if (pProp->id != GUM_Table_Property_Id_rowHeight)
        return GBL_RESULT_ERROR_INVALID_PROPERTY;

    GUM_TABLE(pObject)->rowHeight = GBL_MAX(GblVariant_float(pValue), 1.0f);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Table_GblObject_property_(const GblObject* pObject,
                                                 const GblProperty* pProp,
                                                 GblVariant* pValue) {
    if (pProp->id != GUM_Table_Property_Id_rowHeight)
        return GBL_RESULT_ERROR_INVALID_PROPERTY;

    return GblVariant_setFloat(pValue, GUM_TABLE(pObject)->rowHeight);
}

static size_t GUM_Table_rowCount_(const GUM_Table* pSelf) {
    const GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    return pSelf_->pModel ? GUM_IItemModel_rowCount(pSelf_->pModel, GUM_MODEL_INDEX_INVALID)
                          : 0;
}

static size_t GUM_Table_columnCount_(const GUM_Table* pSelf) {
    const GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    return pSelf_->pModel ? GUM_IItemModel_columnCount(pSelf_->pModel, GUM_MODEL_INDEX_INVALID)
                          : 0;
}

static GblBool GUM_Table_rowForIndex_(const GUM_Table* pSelf,
                                      GUM_ModelIndex target,
                                      size_t* pRow) {
    const GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    if (!pSelf_->pModel || !GUM_ModelIndex_belongs(target, pSelf_->pModel))
        return GBL_FALSE;

    const size_t rows = GUM_Table_rowCount_(pSelf);
    for (size_t row = 0; row < rows; ++row) {
        const GUM_ModelIndex index = GUM_IItemModel_index(pSelf_->pModel,
                                                          row,
                                                          target.column,
                                                          GUM_MODEL_INDEX_INVALID);
        if (GUM_ModelIndex_equal(index, target)) {
            if (pRow)
                *pRow = row;
            return GBL_TRUE;
        }
    }
    return GBL_FALSE;
}

static void GUM_Table_revealRow_(GUM_Table* pSelf, size_t row) {
    GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
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

static GUM_ModelIndex GUM_Table_firstSelectable_(const GUM_Table* pSelf) {
    const GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    const size_t rows = GUM_Table_rowCount_(pSelf);
    const size_t columns = GUM_Table_columnCount_(pSelf);

    for (size_t row = 0; row < rows; ++row) {
        for (size_t column = 0; column < columns; ++column) {
            const GUM_ModelIndex index = GUM_IItemModel_index(pSelf_->pModel,
                                                              row,
                                                              column,
                                                              GUM_MODEL_INDEX_INVALID);
            if (GUM_ModelIndex_valid(index) &&
                (GUM_IItemModel_flags(pSelf_->pModel, index) & GUM_ITEM_SELECTABLE))
                return index;
        }
    }
    return GUM_MODEL_INDEX_INVALID;
}

static GUM_ModelIndex GUM_Table_moveVertical_(const GUM_Table* pSelf,
                                              size_t row,
                                              size_t column,
                                              int direction) {
    const GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    const size_t rows = GUM_Table_rowCount_(pSelf);
    if (!rows)
        return GUM_MODEL_INDEX_INVALID;

    if (direction < 0) {
        for (size_t current = row; current > 0; --current) {
            const GUM_ModelIndex index = GUM_IItemModel_index(pSelf_->pModel,
                                                              current - 1,
                                                              column,
                                                              GUM_MODEL_INDEX_INVALID);
            if (GUM_ModelIndex_valid(index) &&
                (GUM_IItemModel_flags(pSelf_->pModel, index) & GUM_ITEM_SELECTABLE))
                return index;
        }
    } else {
        for (size_t current = row + 1; current < rows; ++current) {
            const GUM_ModelIndex index = GUM_IItemModel_index(pSelf_->pModel,
                                                              current,
                                                              column,
                                                              GUM_MODEL_INDEX_INVALID);
            if (GUM_ModelIndex_valid(index) &&
                (GUM_IItemModel_flags(pSelf_->pModel, index) & GUM_ITEM_SELECTABLE))
                return index;
        }
    }
    return GUM_MODEL_INDEX_INVALID;
}

static GUM_ModelIndex GUM_Table_moveHorizontal_(const GUM_Table* pSelf,
                                                size_t row,
                                                size_t column,
                                                int direction) {
    const GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    const size_t columns = GUM_Table_columnCount_(pSelf);
    if (!columns)
        return GUM_MODEL_INDEX_INVALID;

    if (direction < 0) {
        for (size_t current = column; current > 0; --current) {
            const GUM_ModelIndex index = GUM_IItemModel_index(pSelf_->pModel,
                                                              row,
                                                              current - 1,
                                                              GUM_MODEL_INDEX_INVALID);
            if (GUM_ModelIndex_valid(index) &&
                (GUM_IItemModel_flags(pSelf_->pModel, index) & GUM_ITEM_SELECTABLE))
                return index;
        }
    } else {
        for (size_t current = column + 1; current < columns; ++current) {
            const GUM_ModelIndex index = GUM_IItemModel_index(pSelf_->pModel,
                                                              row,
                                                              current,
                                                              GUM_MODEL_INDEX_INVALID);
            if (GUM_ModelIndex_valid(index) &&
                (GUM_IItemModel_flags(pSelf_->pModel, index) & GUM_ITEM_SELECTABLE))
                return index;
        }
    }
    return GUM_MODEL_INDEX_INVALID;
}

static GBL_RESULT GUM_Table_update_(GUM_Widget* pWidget) {
    GUM_Table* pSelf = GUM_TABLE(pWidget);
    GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    const float contentHeight = (float)GUM_Table_rowCount_(pSelf) * pSelf->rowHeight;

    GUM_ScrollViewport_setRange_(&pSelf_->viewport,
                                 GUM_SCROLL_AXIS_Y,
                                 GBL_MAX(contentHeight - pWidget->h, 0.0f));
    GUM_ScrollViewport_update_(&pSelf_->viewport, GUM_Backend_frametime());
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Table_draw_(GUM_Widget* pWidget, GUM_Renderer* pRenderer) {
    GBL_RESULT result = GUM_WIDGET_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE))->pFnDraw(pWidget,
                                                                                            pRenderer);
    GUM_Table* pSelf = GUM_TABLE(pWidget);
    GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result) || !pSelf_->pModel)
        return result;

    const size_t rows = GUM_Table_rowCount_(pSelf);
    const size_t columns = GUM_Table_columnCount_(pSelf);
    if (!rows || !columns)
        return GBL_RESULT_SUCCESS;

    const GUM_Vector2 position = GUM_get_absolute_position_(pWidget);
    const GUM_Rectangle rec = { position.x, position.y, pWidget->w, pWidget->h };
    const GUM_Rectangle clip = GUM_Rectangle_intersect(GUM_Widget_clipRect_(pWidget), rec);
    if (clip.width <= 0.0f || clip.height <= 0.0f)
        return GBL_RESULT_SUCCESS;

    result = GUM_Backend_beginScissor(pRenderer, clip);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    const float scroll = GUM_ScrollViewport_position_(&pSelf_->viewport, GUM_SCROLL_AXIS_Y);
    const float columnWidth = columns ? rec.width / (float)columns : rec.width;
    const size_t firstRow = pSelf->rowHeight > 0.0f ? (size_t)(scroll / pSelf->rowHeight) : 0;
    const size_t visibleRows = pSelf->rowHeight > 0.0f
                             ? (size_t)(GBL_MAX(rec.height, 0.0f) / pSelf->rowHeight) + 2
                             : rows;
    const size_t lastRow = GBL_MIN(firstRow + visibleRows, rows);

    for (size_t row = firstRow; row < lastRow; ++row) {
        const float y = rec.y + (float)row * pSelf->rowHeight - scroll;

        for (size_t column = 0; column < columns; ++column) {
            const GUM_ModelIndex index = GUM_IItemModel_index(pSelf_->pModel,
                                                              row,
                                                              column,
                                                              GUM_MODEL_INDEX_INVALID);
            if (!GUM_ModelIndex_valid(index))
                continue;

            const GUM_Rectangle cell = {
                rec.x + (float)column * columnWidth,
                y,
                columnWidth,
                pSelf->rowHeight
            };

            if (GUM_ModelIndex_equal(index, pSelf_->selection)) {
                result = GUM_Backend_rectangleDraw(pRenderer,
                                                   cell,
                                                   0.0f,
                                                   (GUM_Color){ pWidget->font_r,
                                                                pWidget->font_g,
                                                                pWidget->font_b,
                                                                48 });
                if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
                    goto cleanup;
            }

            GBL_VARIANT(value);
            result = GUM_IItemModel_data(pSelf_->pModel, index, &value);
            if (GBL_RESULT_SUCCESS(result)) {
                GblStringRef* pText = GblVariant_toString(&value);
                if (pText) {
                    result = GUM_Backend_Font_draw(pRenderer,
                                                   GUM_Widget_font(pWidget),
                                                   pText,
                                                   (GUM_Vector2){ cell.x + 4.0f,
                                                                  y + (pSelf->rowHeight - pWidget->font_size) * 0.5f },
                                                   (GUM_Color){ pWidget->font_r,
                                                                pWidget->font_g,
                                                                pWidget->font_b,
                                                                pWidget->font_a },
                                                   pWidget->font_size,
                                                   1.2f);
                }
            }
            GblVariant_destruct(&value);
            if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
                goto cleanup;
        }
    }

cleanup: {
        const GBL_RESULT endResult = GUM_Backend_endScissor(pRenderer);
        return GBL_RESULT_SUCCESS(result) ? endResult : result;
    }
}

static GBL_RESULT GUM_Table_pointerPress_(GUM_Table* pSelf, GUM_Event_Mouse* pEvent) {
    GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    if (!pSelf_->pModel || GUM_EVENT_INPUT(pEvent)->state != GUM_INPUTSTATE_PRESS ||
        GUM_EVENT_INPUT(pEvent)->button != GUM_MOUSE_BUTTON_LEFT)
        return GBL_RESULT_PARTIAL;

    const size_t rows = GUM_Table_rowCount_(pSelf);
    const size_t columns = GUM_Table_columnCount_(pSelf);
    if (!rows || !columns || pSelf->rowHeight <= 0.0f || GUM_WIDGET(pSelf)->w <= 0.0f)
        return GBL_RESULT_PARTIAL;

    const GUM_Vector2 position = GUM_get_absolute_position_(GUM_WIDGET(pSelf));
    const GUM_Vector2 pointer = GUM_EVENT_POINTER(pEvent)->position;
    if (pointer.x < position.x || pointer.x >= position.x + GUM_WIDGET(pSelf)->w ||
        pointer.y < position.y || pointer.y >= position.y + GUM_WIDGET(pSelf)->h)
        return GBL_RESULT_PARTIAL;

    const float localY = pointer.y - position.y
                       + GUM_ScrollViewport_position_(&pSelf_->viewport, GUM_SCROLL_AXIS_Y);
    const size_t row = (size_t)(localY / pSelf->rowHeight);
    const float columnWidth = GUM_WIDGET(pSelf)->w / (float)columns;
    const size_t column = (size_t)((pointer.x - position.x) / columnWidth);
    if (row >= rows || column >= columns)
        return GBL_RESULT_PARTIAL;

    const GUM_ModelIndex index = GUM_IItemModel_index(pSelf_->pModel,
                                                      row,
                                                      column,
                                                      GUM_MODEL_INDEX_INVALID);
    if (!GUM_ModelIndex_valid(index) ||
        !(GUM_IItemModel_flags(pSelf_->pModel, index) & GUM_ITEM_SELECTABLE))
        return GBL_RESULT_PARTIAL;

    return GUM_Table_select(pSelf, index);
}

static GBL_RESULT GUM_Table_inputEvent_(GUM_Widget* pWidget, GUM_Event_Input* pEvent) {
    GUM_Table* pSelf = GUM_TABLE(pWidget);
    GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
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
        const GBL_RESULT result = GUM_Table_pointerPress_(pSelf, pMouseEvent);
        if (result != GBL_RESULT_PARTIAL && !GBL_RESULT_SUCCESS(result))
            return result;
    }

    return GUM_WIDGET_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE))->pFnInputEvent(pWidget,
                                                                                     pEvent);
}

static GblBool GUM_Table_navigate_(GUM_Widget* pWidget, GUM_InputAction action) {
    GUM_Table* pSelf = GUM_TABLE(pWidget);
    GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    if (!pSelf_->pModel || !GUM_Table_rowCount_(pSelf) || !GUM_Table_columnCount_(pSelf))
        return GBL_FALSE;

    if (!GUM_ModelIndex_valid(pSelf_->selection)) {
        const GUM_ModelIndex first = GUM_Table_firstSelectable_(pSelf);
        if (!GUM_ModelIndex_valid(first))
            return GBL_FALSE;

        size_t row = 0;
        GUM_Table_select(pSelf, first);
        if (GUM_Table_rowForIndex_(pSelf, first, &row))
            GUM_Table_revealRow_(pSelf, row);
        return GBL_TRUE;
    }

    size_t row = 0;
    if (!GUM_Table_rowForIndex_(pSelf, pSelf_->selection, &row))
        return GBL_FALSE;

    GUM_ModelIndex target = GUM_MODEL_INDEX_INVALID;
    switch (action) {
        case GUM_INPUTACTION_MOVE_UP:
            target = GUM_Table_moveVertical_(pSelf, row, pSelf_->selection.column, -1);
            break;
        case GUM_INPUTACTION_MOVE_DOWN:
            target = GUM_Table_moveVertical_(pSelf, row, pSelf_->selection.column, 1);
            break;
        case GUM_INPUTACTION_MOVE_LEFT:
            target = GUM_Table_moveHorizontal_(pSelf, row, pSelf_->selection.column, -1);
            break;
        case GUM_INPUTACTION_MOVE_RIGHT:
            target = GUM_Table_moveHorizontal_(pSelf, row, pSelf_->selection.column, 1);
            break;
        default:
            return GBL_FALSE;
    }

    if (!GUM_ModelIndex_valid(target))
        return GBL_FALSE;

    GUM_Table_select(pSelf, target);
    if (GUM_Table_rowForIndex_(pSelf, target, &row))
        GUM_Table_revealRow_(pSelf, row);
    return GBL_TRUE;
}

static GBL_RESULT GUM_TableClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_TABLE_TYPE)) {
        GBL_PROPERTIES_REGISTER(GUM_Table);

        GblSignal_install(GUM_TABLE_TYPE, "selectionChanged",
                          GUM_Marshal_CClosure_VOID__INSTANCE_POINTER_SIZE_, 2,
                          GBL_POINTER_TYPE, GBL_SIZE_TYPE);
    }

    GBL_BOX_CLASS(pClass)->pFnDestructor      = GUM_Table_GblBox_destructor_;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GUM_Table_GblObject_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty    = GUM_Table_GblObject_property_;
    GUM_WIDGET_CLASS(pClass)->pFnUpdate      = GUM_Table_update_;
    GUM_WIDGET_CLASS(pClass)->pFnDraw        = GUM_Table_draw_;
    GUM_WIDGET_CLASS(pClass)->pFnInputEvent  = GUM_Table_inputEvent_;
    GUM_WIDGET_CLASS(pClass)->pFnNavigate    = GUM_Table_navigate_;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_TableClass_final_(GblClass* pClass, const void* pClassData) {
    GBL_UNUSED(pClassData);

    if (!GblType_classRefCount(GUM_TABLE_TYPE)) {
        GblProperty_uninstallAll(GUM_TABLE_TYPE);
        GblSignal_uninstall(GUM_TABLE_TYPE, "selectionChanged");
    }

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GUM_IItemModel* GUM_Table_model(const GUM_Table* pSelf) {
    return pSelf ? GUM_TABLE_(pSelf)->pModel : nullptr;
}

GBL_EXPORT GBL_RESULT GUM_Table_setModel(GUM_Table* pSelf, GUM_IItemModel* pModel) {
    if (!pSelf)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    if (pSelf_->pModel == pModel)
        return GBL_RESULT_SUCCESS;

    if (pModel) {
        GUM_IItemModel_ref(pModel);
        const GBL_RESULT result = GblSignal_connect(GBL_INSTANCE(pModel),
                                                    "structureChanged",
                                                    GBL_INSTANCE(pSelf),
                                                    (GblFnPtr)GUM_Table_structureChanged_,
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
    GUM_ScrollViewport_reset_(&pSelf_->viewport);

    return hadSelection ? GBL_EMIT(pSelf, "selectionChanged", nullptr, (size_t)0)
                        : GBL_RESULT_SUCCESS;
}

GBL_EXPORT GUM_ModelIndex GUM_Table_selection(const GUM_Table* pSelf) {
    return pSelf ? GUM_TABLE_(pSelf)->selection : GUM_MODEL_INDEX_INVALID;
}

GBL_EXPORT GBL_RESULT GUM_Table_select(GUM_Table* pSelf, GUM_ModelIndex index) {
    if (!pSelf)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_Table_* pSelf_ = GUM_TABLE_(pSelf);
    if (GUM_ModelIndex_valid(index)) {
        if (!pSelf_->pModel ||
            !GUM_ModelIndex_belongs(index, pSelf_->pModel) ||
            !(GUM_IItemModel_flags(pSelf_->pModel, index) & GUM_ITEM_SELECTABLE))
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

GBL_EXPORT float GUM_Table_scrollPosition(const GUM_Table* pSelf) {
    return pSelf ? GUM_ScrollViewport_position_(&GUM_TABLE_(pSelf)->viewport, GUM_SCROLL_AXIS_Y)
                 : 0.0f;
}

GBL_EXPORT float GUM_Table_scrollRange(const GUM_Table* pSelf) {
    return pSelf ? GUM_ScrollViewport_range_(&GUM_TABLE_(pSelf)->viewport, GUM_SCROLL_AXIS_Y)
                 : 0.0f;
}

GBL_EXPORT void GUM_Table_scrollTo(GUM_Table* pSelf, float position) {
    if (pSelf)
        GUM_ScrollViewport_scrollTo_(&GUM_TABLE_(pSelf)->viewport, GUM_SCROLL_AXIS_Y, position);
}

GBL_EXPORT void GUM_Table_scrollBy(GUM_Table* pSelf, float delta) {
    if (pSelf)
        GUM_ScrollViewport_scrollBy_(&GUM_TABLE_(pSelf)->viewport, GUM_SCROLL_AXIS_Y, delta);
}

GBL_EXPORT GblType GUM_Table_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if GBL_UNLIKELY (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Table"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){
                                    .classSize           = sizeof(GUM_TableClass),
                                    .instanceSize        = sizeof(GUM_Table),
                                    .instancePrivateSize = sizeof(GUM_Table_),
                                    .pFnClassInit        = GUM_TableClass_init_,
                                    .pFnInstanceInit     = GUM_Table_init_,
                                    .pFnClassFinal       = GUM_TableClass_final_
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}
