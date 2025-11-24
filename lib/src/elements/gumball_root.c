#include <gumball/elements/gumball_widget.h>
#include <gumball/elements/gumball_root.h>
#include <gumball/types/gumball_renderer.h>
#include <gumball/core/gumball_backend.h>

#include <gimbal/gimbal_algorithms.h>


static GblArrayList GUM_drawQueue_;
static GblLogger    *pLogger_ = nullptr;

static GBL_RESULT GUM_Root_init_(GblInstance* pInstance) {
    GblObject_setName(GBL_OBJECT(pInstance), "GUM_Root");
    if (!GblModule_isLoaded(GBL_MODULE(pInstance))) {
        GblModule_register(GBL_MODULE(pInstance));
    }
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_RootClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_ROOT_TYPE)) {
        if GBL_UNLIKELY(!pLogger_)
            pLogger_ = GblLogger_create(GBL_LOGGER_TYPE, sizeof(GblLogger), nullptr);

        GUM_drawQueue_init();
	    GblLogger_register(pLogger_);
        GUM_Backend_setLogger();
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_RootClass_final_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_ROOT_TYPE)) {
        GUM_drawQueue_free();
        GblLogger_unregister(pLogger_);
        GUM_Backend_resetLogger();
    }

    return GBL_RESULT_SUCCESS;
}

GblType GUM_Root_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Root"),
                                GBL_MODULE_TYPE,
                                &(static GblTypeInfo){.classSize = sizeof(GUM_RootClass),
                                                      .instanceSize = sizeof(GUM_Root),
                                                      .pFnClassInit = GUM_RootClass_init_,
                                                      .pFnClassFinal = GUM_RootClass_final_,
                                                      .pFnInstanceInit = GUM_Root_init_},
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

static int GUM_zIndex_cmp_(const void *pA, const void *pB) {
    GblObject *a = *(GblObject**)pA;
    GblObject *b = *(GblObject**)pB;

    GUM_Widget *aWidget = GBL_AS(GUM_Widget, a);
    GUM_Widget *bWidget = GBL_AS(GUM_Widget, b);

    if (! aWidget || ! bWidget) return 0;

    return aWidget->z_index - bWidget->z_index;
}

void GUM_drawQueue_sort(void) {
    gblSortInsertion(GblArrayList_data(&GUM_drawQueue_), GblArrayList_size(&GUM_drawQueue_), sizeof(GblObject*), GUM_zIndex_cmp_);
}

GblArrayList *GUM_drawQueue_get(void) {
	return &GUM_drawQueue_;
}

void GUM_drawQueue_init(void) {
    GblArrayList_construct(&GUM_drawQueue_, sizeof(GblObject*));
}

void GUM_drawQueue_free(void) {
    GblArrayList_destruct(&GUM_drawQueue_);
}

void GUM_drawQueue_push(GblObject *pObj) {
    if (!pObj) return;
    GUM_Widget *pWidget = GBL_AS(GUM_Widget, pObj);
    if (!pWidget) return;

    GblArrayList_pushBack(&GUM_drawQueue_, &pObj);
    GUM_drawQueue_sort();
}

void GUM_drawQueue_remove(GblObject *pObj) {
    if (!pObj) return;
    if (!GBL_AS(GUM_Widget, pObj)) return;

    for (size_t i = 0; i < GblArrayList_size(&GUM_drawQueue_); i++) {
        GblObject *pObjQueue = *(GblObject**)GblArrayList_at(&GUM_drawQueue_, i);
        if (pObjQueue == pObj) {
            GblArrayList_erase(&GUM_drawQueue_, i, 1);
            GUM_drawQueue_sort();
            return;
        }
    }
}