#include "elements/gumball_modelview_test_suite.h"

#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#include <string.h>

#define GUM_PROPERTY_OVERRIDE_WIDGET_TYPE          (GBL_TYPEID     (GUM_PropertyOverrideWidget))
#define GUM_PROPERTY_OVERRIDE_WIDGET(self)         (GBL_CAST       (GUM_PropertyOverrideWidget, self))

GBL_FORWARD_DECLARE_STRUCT(GUM_PropertyOverrideWidget);
GblType GUM_PropertyOverrideWidget_type(void) GBL_NOEXCEPT;

GBL_CLASS_DERIVE_EMPTY(GUM_PropertyOverrideWidget, GUM_Widget)
GBL_INSTANCE_DERIVE_EMPTY(GUM_PropertyOverrideWidget, GUM_Widget)

GBL_PROPERTIES(GUM_PropertyOverrideWidget,
    (x, GBL_GENERIC, (READ, WRITE, OVERRIDE), GBL_FLOAT_TYPE)
)

static GBL_RESULT GUM_PropertyOverrideWidget_setProperty_(GblObject* pObject,
                                                           const GblProperty* pProp,
                                                           GblVariant* pValue) {
    if (pProp->id == GUM_PropertyOverrideWidget_Property_Id_x) {
        GUM_WIDGET(pObject)->x = GblVariant_float(pValue);
        return GBL_RESULT_SUCCESS;
    }
    return GBL_RESULT_ERROR_INVALID_PROPERTY;
}

static GBL_RESULT GUM_PropertyOverrideWidget_property_(const GblObject* pObject,
                                                        const GblProperty* pProp,
                                                        GblVariant* pValue) {
    if (pProp->id == GUM_PropertyOverrideWidget_Property_Id_x)
        return GblVariant_setFloat(pValue, GUM_WIDGET(pObject)->x);
    return GBL_RESULT_ERROR_INVALID_PROPERTY;
}

static GBL_RESULT GUM_PropertyOverrideWidgetClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    if (!GblType_classRefCount(GUM_PROPERTY_OVERRIDE_WIDGET_TYPE))
        GBL_PROPERTIES_REGISTER(GUM_PropertyOverrideWidget);

    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GUM_PropertyOverrideWidget_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty = GUM_PropertyOverrideWidget_property_;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_PropertyOverrideWidgetClass_final_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pClass, pData);
    if (!GblType_classRefCount(GUM_PROPERTY_OVERRIDE_WIDGET_TYPE))
        GblProperty_uninstallAll(GUM_PROPERTY_OVERRIDE_WIDGET_TYPE);
    return GBL_RESULT_SUCCESS;
}

GblType GUM_PropertyOverrideWidget_type(void) {
    static GblType type = GBL_INVALID_TYPE;
    if GBL_UNLIKELY (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_PropertyOverrideWidget"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){
                                    .classSize = sizeof(GUM_PropertyOverrideWidgetClass),
                                    .instanceSize = sizeof(GUM_PropertyOverrideWidget),
                                    .pFnClassInit = GUM_PropertyOverrideWidgetClass_init_,
                                    .pFnClassFinal = GUM_PropertyOverrideWidgetClass_final_
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}

#define GUM_LARGE_HIERARCHY_MODEL_TYPE          (GBL_TYPEID     (GUM_LargeHierarchyModel))
#define GUM_LARGE_HIERARCHY_MODEL(self)         (GBL_CAST       (GUM_LargeHierarchyModel, self))

GBL_FORWARD_DECLARE_STRUCT(GUM_LargeHierarchyModel);
GblType GUM_LargeHierarchyModel_type(void) GBL_NOEXCEPT;

GBL_CLASS_DERIVE_EMPTY(GUM_LargeHierarchyModel, GblBox, GUM_IItemModel)
GBL_INSTANCE_DERIVE_EMPTY(GUM_LargeHierarchyModel, GblBox)

enum { GUM_LARGE_HIERARCHY_CHILD_COUNT_ = 1024 };
static char largeHierarchyHandles_[GUM_LARGE_HIERARCHY_CHILD_COUNT_ + 1];

static GUM_ModelIndex GUM_LargeHierarchyModel_indexFor_(const GUM_IItemModel* pModel,
                                                         const void* pHandle) {
    return pHandle ? (GUM_ModelIndex){ .pModel = pModel, .pHandle = pHandle }
                   : GUM_MODEL_INDEX_INVALID;
}

static size_t GUM_LargeHierarchyModel_columnCount_(const GUM_IItemModel* pModel,
                                                    GUM_ModelIndex parent) {
    GBL_UNUSED(pModel, parent);
    return 1;
}

static size_t GUM_LargeHierarchyModel_rowCount_(const GUM_IItemModel* pModel,
                                                 GUM_ModelIndex parent) {
    GBL_UNUSED(pModel);
    if (!GUM_ModelIndex_valid(parent))
        return 1;
    return parent.pHandle == &largeHierarchyHandles_[0]
         ? GUM_LARGE_HIERARCHY_CHILD_COUNT_
         : 0;
}

static GUM_ModelIndex GUM_LargeHierarchyModel_index_(const GUM_IItemModel* pModel,
                                                      size_t row,
                                                      size_t column,
                                                      GUM_ModelIndex parent) {
    if (column)
        return GUM_MODEL_INDEX_INVALID;

    if (!GUM_ModelIndex_valid(parent))
        return row == 0 ? GUM_LargeHierarchyModel_indexFor_(pModel, &largeHierarchyHandles_[0])
                        : GUM_MODEL_INDEX_INVALID;

    if (parent.pHandle == &largeHierarchyHandles_[0] &&
        row < GUM_LARGE_HIERARCHY_CHILD_COUNT_) {
        return GUM_LargeHierarchyModel_indexFor_(pModel, &largeHierarchyHandles_[row + 1]);
    }
    return GUM_MODEL_INDEX_INVALID;
}

static GUM_ModelIndex GUM_LargeHierarchyModel_parent_(const GUM_IItemModel* pModel,
                                                       GUM_ModelIndex index) {
    if (index.pHandle == &largeHierarchyHandles_[0])
        return GUM_MODEL_INDEX_INVALID;
    return GUM_LargeHierarchyModel_indexFor_(pModel, &largeHierarchyHandles_[0]);
}

static GBL_RESULT GUM_LargeHierarchyModel_data_(const GUM_IItemModel* pModel,
                                                 GUM_ModelIndex index,
                                                 GblVariant* pValue) {
    GBL_UNUSED(pModel);
    return GblVariant_setString(pValue,
                                index.pHandle == &largeHierarchyHandles_[0] ? "root" : "row");
}

static GBL_RESULT GUM_LargeHierarchyModel_setData_(GUM_IItemModel* pModel,
                                                    GUM_ModelIndex index,
                                                    GblVariant* pValue) {
    GBL_UNUSED(pModel, index, pValue);
    return GBL_RESULT_UNSUPPORTED;
}

static GblFlags GUM_LargeHierarchyModel_flags_(const GUM_IItemModel* pModel,
                                                GUM_ModelIndex index) {
    GBL_UNUSED(pModel, index);
    return GUM_ITEM_SELECTABLE;
}

static GBL_RESULT GUM_LargeHierarchyModelClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GUM_IItemModelClass* pModel = GUM_IITEMMODEL_CLASS(pClass);
    pModel->pFnColumnCount = GUM_LargeHierarchyModel_columnCount_;
    pModel->pFnRowCount    = GUM_LargeHierarchyModel_rowCount_;
    pModel->pFnIndex       = GUM_LargeHierarchyModel_index_;
    pModel->pFnParent      = GUM_LargeHierarchyModel_parent_;
    pModel->pFnData        = GUM_LargeHierarchyModel_data_;
    pModel->pFnSetData     = GUM_LargeHierarchyModel_setData_;
    pModel->pFnFlags       = GUM_LargeHierarchyModel_flags_;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_LargeHierarchyModel_type(void) {
    static GblType type = GBL_INVALID_TYPE;
    static GblInterfaceImpl interfaces[1];

    if GBL_UNLIKELY (type == GBL_INVALID_TYPE) {
        interfaces[0] = (GblInterfaceImpl){
            .interfaceType = GUM_IITEMMODEL_TYPE,
            .classOffset = offsetof(GUM_LargeHierarchyModelClass, GUM_IItemModelImpl)
        };
        type = GblType_register(GblQuark_internStatic("GUM_LargeHierarchyModel"),
                                GBL_BOX_TYPE,
                                &(static GblTypeInfo){
                                    .classSize = sizeof(GUM_LargeHierarchyModelClass),
                                    .pFnClassInit = GUM_LargeHierarchyModelClass_init_,
                                    .instanceSize = sizeof(GUM_LargeHierarchyModel),
                                    .pInterfaceImpls = interfaces,
                                    .interfaceCount = GBL_COUNT_OF(interfaces)
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}

static GUM_LargeHierarchyModel* GUM_LargeHierarchyModel_create_(void) {
    return GUM_LARGE_HIERARCHY_MODEL(
        GblBox_create(GUM_LARGE_HIERARCHY_MODEL_TYPE,
                      sizeof(GUM_LargeHierarchyModel),
                      nullptr,
                      nullptr,
                      nullptr));
}

#define GBL_SELF_TYPE GUM_ModelViewTestSuite

GBL_TEST_FIXTURE {
    GUM_Root* pRoot;
};

static size_t modelDataChangedCount_ = 0;
static const void* pModelChangedHandle_ = nullptr;
static size_t modelChangedColumn_ = 0;
static size_t modelStructureChangedCount_ = 0;
static size_t selectionChangedCount_ = 0;
static const void* pSelectionHandle_ = nullptr;
static size_t selectionColumn_ = 0;

static void modelDataChanged_(GblInstance* pReceiver, void* pHandle, size_t column) {
    GBL_UNUSED(pReceiver);
    ++modelDataChangedCount_;
    pModelChangedHandle_ = pHandle;
    modelChangedColumn_ = column;
}

static void modelStructureChanged_(GblInstance* pReceiver) {
    GBL_UNUSED(pReceiver);
    ++modelStructureChangedCount_;
}

static void selectionChanged_(GblInstance* pReceiver, void* pHandle, size_t column) {
    GBL_UNUSED(pReceiver);
    ++selectionChangedCount_;
    pSelectionHandle_ = pHandle;
    selectionColumn_ = column;
}

static void countersReset_(void) {
    modelDataChangedCount_ = 0;
    pModelChangedHandle_ = nullptr;
    modelChangedColumn_ = 0;
    modelStructureChangedCount_ = 0;
    selectionChangedCount_ = 0;
    pSelectionHandle_ = nullptr;
    selectionColumn_ = 0;
}

static GUM_ModelIndex findProperty_(GUM_PropertyModel* pModel,
                                    const char* pName,
                                    size_t column) {
    GUM_IItemModel* pItemModel = GUM_IITEMMODEL(pModel);
    const size_t rows = GUM_IItemModel_rowCount(pItemModel, GUM_MODEL_INDEX_INVALID);

    for (size_t row = 0; row < rows; ++row) {
        const GUM_ModelIndex nameIndex = GUM_IItemModel_index(pItemModel,
                                                              row,
                                                              0,
                                                              GUM_MODEL_INDEX_INVALID);
        GBL_VARIANT(value);
        const GBL_RESULT result = GUM_IItemModel_data(pItemModel, nameIndex, &value);
        if (GBL_RESULT_SUCCESS(result)) {
            GblStringRef* pValue = GblVariant_string(&value);
            if (pValue && strcmp(pValue, pName) == 0) {
                GblVariant_destruct(&value);
                return GUM_IItemModel_index(pItemModel,
                                            row,
                                            column,
                                            GUM_MODEL_INDEX_INVALID);
            }
        }
        GblVariant_destruct(&value);
    }
    return GUM_MODEL_INDEX_INVALID;
}

static size_t propertyCount_(GUM_PropertyModel* pModel, const char* pName) {
    GUM_IItemModel* pItemModel = GUM_IITEMMODEL(pModel);
    const size_t rows = GUM_IItemModel_rowCount(pItemModel, GUM_MODEL_INDEX_INVALID);
    size_t count = 0;

    for (size_t row = 0; row < rows; ++row) {
        const GUM_ModelIndex index = GUM_IItemModel_index(pItemModel,
                                                          row,
                                                          0,
                                                          GUM_MODEL_INDEX_INVALID);
        GBL_VARIANT(value);
        if (GBL_RESULT_SUCCESS(GUM_IItemModel_data(pItemModel, index, &value)) &&
            strcmp(GblVariant_string(&value), pName) == 0)
            ++count;
        GblVariant_destruct(&value);
    }
    return count;
}

GBL_TEST_INIT()
    countersReset_();
    pFixture->pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    countersReset_();
    GUM_unref(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(objectTreeModel)
    countersReset_();

    GblObject* pRoot = GBL_NEW(GblObject, "name", "root");
    GblObject* pChild = GBL_NEW(GblObject, "name", "child");
    GblObject* pGrandchild = GBL_NEW(GblObject, "name", "grandchild");
    GblObject* pOtherRoot = GBL_NEW(GblObject, "name", "other");
    GBL_TEST_VERIFY(pRoot && pChild && pGrandchild && pOtherRoot);

    GblObject_addChild(pRoot, pChild);
    GblObject_addChild(pChild, pGrandchild);

    GUM_ObjectTreeModel* pModel = GUM_ObjectTreeModel_create(pRoot);
    GUM_ObjectTreeModel* pOtherModel = GUM_ObjectTreeModel_create(pOtherRoot);
    GBL_TEST_VERIFY(pModel && pOtherModel);

    GUM_IItemModel* pItemModel = GUM_IITEMMODEL(pModel);
    GUM_IItemModel* pOtherItemModel = GUM_IITEMMODEL(pOtherModel);
    GBL_TEST_COMPARE(GUM_IItemModel_columnCount(pItemModel, GUM_MODEL_INDEX_INVALID), 1u);
    GBL_TEST_COMPARE(GUM_IItemModel_rowCount(pItemModel, GUM_MODEL_INDEX_INVALID), 1u);

    const GUM_ModelIndex rootIndex = GUM_IItemModel_index(pItemModel,
                                                          0,
                                                          0,
                                                          GUM_MODEL_INDEX_INVALID);
    const GUM_ModelIndex childIndex = GUM_IItemModel_index(pItemModel, 0, 0, rootIndex);
    const GUM_ModelIndex grandchildIndex = GUM_IItemModel_index(pItemModel, 0, 0, childIndex);
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(rootIndex));
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(childIndex));
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(grandchildIndex));
    GBL_TEST_VERIFY(GUM_ModelIndex_belongs(childIndex, pItemModel));
    GBL_TEST_VERIFY(!GUM_ModelIndex_belongs(childIndex, pOtherItemModel));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_IItemModel_parent(pItemModel, childIndex), rootIndex));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_IItemModel_parent(pItemModel, grandchildIndex), childIndex));

    GBL_VARIANT(value);
    GBL_TEST_CALL(GUM_IItemModel_data(pItemModel, childIndex, &value));
    GBL_TEST_COMPARE(strcmp(GblVariant_string(&value), "child"), 0);
    GblVariant_destruct(&value);

    // Models that do not override displayData inherit their typed data unchanged.
    GBL_VARIANT(displayValue);
    GBL_TEST_CALL(GUM_IItemModel_displayData(pItemModel, childIndex, &displayValue));
    GBL_TEST_COMPARE(strcmp(GblVariant_string(&displayValue), "child"), 0);
    GblVariant_destruct(&displayValue);

    GBL_VARIANT(foreignValue);
    GBL_TEST_VERIFY(!GBL_RESULT_SUCCESS(GUM_IItemModel_data(pOtherItemModel,
                                                            childIndex,
                                                            &foreignValue)));
    GblVariant_destruct(&foreignValue);

    GBL_TEST_CALL(GblSignal_connect(GBL_INSTANCE(pItemModel),
                                    "dataChanged",
                                    GBL_INSTANCE(pFixture->pRoot),
                                    (GblFnPtr)modelDataChanged_,
                                    nullptr));
    GBL_VARIANT(name);
    GBL_TEST_CALL(GblVariant_setString(&name, "renamed"));
    GBL_TEST_CALL(GUM_IItemModel_setData(pItemModel, childIndex, &name));
    GBL_TEST_COMPARE(strcmp(GblObject_name(pChild), "renamed"), 0);
    GBL_TEST_COMPARE(modelDataChangedCount_, 1u);
    GBL_TEST_VERIFY(pModelChangedHandle_ == childIndex.pHandle);
    GBL_TEST_COMPARE(modelChangedColumn_, 0u);
    GblVariant_destruct(&name);

    // The model owns a live root reference independently of the caller.
    GBL_UNREF(pRoot);
    GBL_VARIANT(rootName);
    GBL_TEST_CALL(GUM_IItemModel_data(pItemModel, rootIndex, &rootName));
    GBL_TEST_COMPARE(strcmp(GblVariant_string(&rootName), "root"), 0);
    GblVariant_destruct(&rootName);

    GUM_IItemModel_unref(pItemModel);
    GUM_IItemModel_unref(pOtherItemModel);
    GBL_UNREF(pOtherRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(propertyModel)
    countersReset_();

    GUM_Widget* pWidget = GUM_Widget_create("x", 11.0f,
                                            "label", "target",
                                            "color", 0x1234ABCDu);
    GUM_Widget* pReplacement = GUM_Widget_create("x", 5.0f);
    GBL_TEST_VERIFY(pWidget && pReplacement);

    GblObject* pPropertyChild = GBL_NEW(GblObject, "name", "property child");
    GblObject* pSecondPropertyChild = GBL_NEW(GblObject, "name", "second child");
    GBL_TEST_VERIFY(pPropertyChild && pSecondPropertyChild);
    GblObject_addChild(GBL_OBJECT(pWidget), pPropertyChild);
    GblObject_addChild(GBL_OBJECT(pWidget), pSecondPropertyChild);

    GUM_PropertyModel* pModel = GUM_PropertyModel_create(GBL_OBJECT(pWidget));
    GBL_TEST_VERIFY(pModel);
    GUM_IItemModel* pItemModel = GUM_IITEMMODEL(pModel);
    GBL_TEST_COMPARE(GUM_IItemModel_columnCount(pItemModel, GUM_MODEL_INDEX_INVALID), 2u);

    // Children are visible by default, but the caller may still hide the collection.
    GBL_TEST_VERIFY(GUM_PropertyModel_propertyVisible(pModel, "children"));
    const GUM_ModelIndex childrenValue = findProperty_(pModel, "children", 1);
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(childrenValue));
    GBL_VARIANT(childrenDisplay);
    GBL_TEST_CALL(GUM_IItemModel_displayData(pItemModel, childrenValue, &childrenDisplay));
    GBL_TEST_COMPARE(strcmp(GblVariant_string(&childrenDisplay),
                            "[property child, second child]"), 0);
    GblVariant_destruct(&childrenDisplay);
    GBL_TEST_CALL(GUM_PropertyModel_setPropertyVisible(pModel, "children", GBL_FALSE));
    GBL_TEST_VERIFY(!GUM_PropertyModel_propertyVisible(pModel, "children"));
    GBL_TEST_VERIFY(!GUM_ModelIndex_valid(findProperty_(pModel, "children", 1)));

    // Acquire indices after the visibility changes, which structurally invalidate old indices.
    const GUM_ModelIndex xName = findProperty_(pModel, "x", 0);
    const GUM_ModelIndex xValue = findProperty_(pModel, "x", 1);
    const GUM_ModelIndex objectNameValue = findProperty_(pModel, "name", 1);
    const GUM_ModelIndex writeOnly = findProperty_(pModel, "labelAcquire", 0);
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(xName));
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(xValue));
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(objectNameValue));
    GBL_TEST_VERIFY(!GUM_ModelIndex_valid(writeOnly));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal((GUM_ModelIndex){ xValue.pModel, xValue.pHandle, 0 }, xName));
    GBL_TEST_VERIFY(GUM_IItemModel_flags(pItemModel, xName) & GUM_ITEM_SELECTABLE);
    GBL_TEST_VERIFY(!(GUM_IItemModel_flags(pItemModel, xName) & GUM_ITEM_EDITABLE));
    GBL_TEST_VERIFY(GUM_IItemModel_flags(pItemModel, xValue) & GUM_ITEM_EDITABLE);

    GBL_VARIANT(value);
    GBL_TEST_CALL(GUM_IItemModel_data(pItemModel, xValue, &value));
    GBL_TEST_COMPARE(GblVariant_float(&value), 11.0f);
    GblVariant_destruct(&value);

    GBL_VARIANT(objectNameDisplay);
    GBL_TEST_CALL(GUM_IItemModel_displayData(pItemModel, objectNameValue, &objectNameDisplay));
    GBL_TEST_COMPARE(strcmp(GblVariant_string(&objectNameDisplay), "GUM_Widget"), 0);
    GblVariant_destruct(&objectNameDisplay);

    // Display formatting never changes the typed edit value.
    const GUM_ModelIndex colorValue = findProperty_(pModel, "color", 1);
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(colorValue));
    GBL_VARIANT(rawColor);
    GBL_TEST_CALL(GUM_IItemModel_data(pItemModel, colorValue, &rawColor));
    GBL_TEST_COMPARE(GblVariant_uint32(&rawColor), (uint32_t)0x1234ABCDu);
    GblVariant_destruct(&rawColor);

    GBL_VARIANT(colorDisplay);
    GBL_TEST_CALL(GUM_IItemModel_displayData(pItemModel, colorValue, &colorDisplay));
    GBL_TEST_COMPARE(strcmp(GblVariant_string(&colorDisplay), "0x1234ABCD"), 0);
    GblVariant_destruct(&colorDisplay);

    const GUM_ModelIndex textureValue = findProperty_(pModel, "texture", 1);
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(textureValue));
    GBL_VARIANT(textureDisplay);
    GBL_TEST_CALL(GUM_IItemModel_displayData(pItemModel, textureValue, &textureDisplay));
    GBL_TEST_COMPARE(strcmp(GblVariant_string(&textureDisplay), "(null)"), 0);
    GblVariant_destruct(&textureDisplay);

    GBL_TEST_VERIFY(GblVariant_canConvert(GUM_FONT_TYPE, GBL_STRING_TYPE));
    GBL_TEST_VERIFY(GblVariant_canConvert(GUM_TEXTURE_TYPE, GBL_STRING_TYPE));

    // Effective reflected names must be unique.
    const size_t rows = GUM_IItemModel_rowCount(pItemModel, GUM_MODEL_INDEX_INVALID);
    for (size_t row = 0; row < rows; ++row) {
        const GUM_ModelIndex lhs = GUM_IItemModel_index(pItemModel,
                                                        row,
                                                        0,
                                                        GUM_MODEL_INDEX_INVALID);
        GBL_VARIANT(lhsValue);
        GBL_TEST_CALL(GUM_IItemModel_data(pItemModel, lhs, &lhsValue));
        for (size_t other = row + 1; other < rows; ++other) {
            const GUM_ModelIndex rhs = GUM_IItemModel_index(pItemModel,
                                                            other,
                                                            0,
                                                            GUM_MODEL_INDEX_INVALID);
            GBL_VARIANT(rhsValue);
            GBL_TEST_CALL(GUM_IItemModel_data(pItemModel, rhs, &rhsValue));
            GBL_TEST_VERIFY(strcmp(GblVariant_string(&lhsValue),
                                   GblVariant_string(&rhsValue)) != 0);
            GblVariant_destruct(&rhsValue);
        }
        GblVariant_destruct(&lhsValue);
    }

    GBL_TEST_CALL(GblSignal_connect(GBL_INSTANCE(pItemModel),
                                    "dataChanged",
                                    GBL_INSTANCE(pFixture->pRoot),
                                    (GblFnPtr)modelDataChanged_,
                                    nullptr));
    GBL_TEST_CALL(GblSignal_connect(GBL_INSTANCE(pItemModel),
                                    "structureChanged",
                                    GBL_INSTANCE(pFixture->pRoot),
                                    (GblFnPtr)modelStructureChanged_,
                                    nullptr));

    GBL_VARIANT(replacementValue);
    GBL_TEST_CALL(GblVariant_setFloat(&replacementValue, 27.0f));
    GBL_TEST_CALL(GUM_IItemModel_setData(pItemModel, xValue, &replacementValue));
    GBL_TEST_COMPARE(pWidget->x, 27.0f);
    GBL_TEST_COMPARE(modelDataChangedCount_, 1u);
    GBL_TEST_VERIFY(pModelChangedHandle_ == xValue.pHandle);
    GBL_TEST_COMPARE(modelChangedColumn_, 1u);
    GblVariant_destruct(&replacementValue);

    // Direct Gimbal variant writes are observable by the model as well.
    GBL_VARIANT(externalValue);
    GBL_TEST_CALL(GblVariant_setFloat(&externalValue, 33.0f));
    GBL_TEST_CALL(GblObject_setPropertyVariant(GBL_OBJECT(pWidget), "x", &externalValue));
    GBL_TEST_COMPARE(modelDataChangedCount_, 2u);
    GBL_TEST_COMPARE(pWidget->x, 33.0f);
    GblVariant_destruct(&externalValue);

    countersReset_();
    GBL_TEST_CALL(GUM_PropertyModel_setObject(pModel, GBL_OBJECT(pReplacement)));
    GBL_TEST_COMPARE(modelStructureChangedCount_, 1u);
    GBL_TEST_VERIFY(GUM_PropertyModel_object(pModel) == GBL_OBJECT(pReplacement));
    GBL_TEST_VERIFY(!GUM_PropertyModel_propertyVisible(pModel, "children"));

    GBL_VARIANT(oldValue);
    GBL_TEST_CALL(GblVariant_setFloat(&oldValue, 44.0f));
    GBL_TEST_CALL(GblObject_setPropertyVariant(GBL_OBJECT(pWidget), "x", &oldValue));
    GBL_TEST_COMPARE(modelDataChangedCount_, 0u);
    GblVariant_destruct(&oldValue);

    const GUM_ModelIndex replacementX = findProperty_(pModel, "x", 1);
    GBL_VARIANT(newValue);
    GBL_TEST_CALL(GblVariant_setFloat(&newValue, 12.0f));
    GBL_TEST_CALL(GblObject_setPropertyVariant(GBL_OBJECT(pReplacement), "x", &newValue));
    GBL_TEST_COMPARE(modelDataChangedCount_, 1u);
    GBL_TEST_VERIFY(pModelChangedHandle_ == replacementX.pHandle);
    GblVariant_destruct(&newValue);

    GUM_IItemModel_unref(pItemModel);
    GUM_unref(pWidget);
    GUM_unref(pReplacement);
GBL_TEST_CASE_END

GBL_TEST_CASE(propertyOverride)
    countersReset_();

    GUM_PropertyOverrideWidget* pWidget = GUM_PROPERTY_OVERRIDE_WIDGET(
        GBL_NEW(GUM_PropertyOverrideWidget, "x", 41.0f));
    GBL_TEST_VERIFY(pWidget);

    GUM_PropertyModel* pModel = GUM_PropertyModel_create(GBL_OBJECT(pWidget));
    GBL_TEST_VERIFY(pModel);
    GUM_IItemModel* pItemModel = GUM_IITEMMODEL(pModel);

    const GUM_ModelIndex xValue = findProperty_(pModel, "x", 1);
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(xValue));
    GBL_TEST_COMPARE(propertyCount_(pModel, "x"), 1u);

    const GblProperty* pProperty = (const GblProperty*)xValue.pHandle;
    GBL_TEST_COMPARE(GblProperty_objectType(pProperty), GUM_PROPERTY_OVERRIDE_WIDGET_TYPE);
    GBL_TEST_VERIFY(pProperty->flags & GBL_PROPERTY_FLAG_OVERRIDE);

    GBL_VARIANT(value);
    GBL_TEST_CALL(GUM_IItemModel_data(pItemModel, xValue, &value));
    GBL_TEST_COMPARE(GblVariant_float(&value), 41.0f);
    GblVariant_destruct(&value);

    GBL_VARIANT(replacement);
    GBL_TEST_CALL(GblVariant_setFloat(&replacement, 52.0f));
    GBL_TEST_CALL(GUM_IItemModel_setData(pItemModel, xValue, &replacement));
    GBL_TEST_COMPARE(GUM_WIDGET(pWidget)->x, 52.0f);
    GblVariant_destruct(&replacement);

    GUM_IItemModel_unref(pItemModel);
    GUM_unref(pWidget);
GBL_TEST_CASE_END

GBL_TEST_CASE(treeView)
    countersReset_();

    GblObject* pRoot = GBL_NEW(GblObject, "name", "root");
    GBL_TEST_VERIFY(pRoot);
    for (size_t i = 0; i < 8; ++i) {
        GblObject* pChild = GBL_NEW(GblObject, "name", "child");
        GBL_TEST_VERIFY(pChild);
        GblObject_addChild(pRoot, pChild);
    }

    GUM_ObjectTreeModel* pModel = GUM_ObjectTreeModel_create(pRoot);
    GUM_Tree* pTree = GUM_Tree_create("h", 48.0f);
    GBL_TEST_VERIFY(pModel && pTree);
    GBL_TEST_VERIFY(GUM_Widget_isActive(GUM_WIDGET(pTree)));
    GBL_TEST_CALL(GUM_Tree_setModel(pTree, GUM_IITEMMODEL(pModel)));

    GBL_TEST_CALL(GblSignal_connect(GBL_INSTANCE(pTree),
                                    "selectionChanged",
                                    GBL_INSTANCE(pFixture->pRoot),
                                    (GblFnPtr)selectionChanged_,
                                    nullptr));

    GUM_IItemModel* pItemModel = GUM_IITEMMODEL(pModel);
    const GUM_ModelIndex rootIndex = GUM_IItemModel_index(pItemModel,
                                                          0,
                                                          0,
                                                          GUM_MODEL_INDEX_INVALID);
    GBL_TEST_VERIFY(GUM_Tree_expanded(pTree, rootIndex));
    GBL_TEST_CALL(GUM_Tree_select(pTree, rootIndex));
    GBL_TEST_COMPARE(selectionChangedCount_, 1u);
    GBL_TEST_VERIFY(pSelectionHandle_ == rootIndex.pHandle);
    GBL_TEST_COMPARE(selectionColumn_, 0u);

    GBL_TEST_CALL(GUM_Tree_setExpanded(pTree, rootIndex, GBL_FALSE));
    GBL_TEST_VERIFY(!GUM_Tree_expanded(pTree, rootIndex));

    // Value-only model changes preserve Tree state.
    GBL_VARIANT(rename);
    GBL_TEST_CALL(GblVariant_setString(&rename, "renamed root"));
    GBL_TEST_CALL(GUM_IItemModel_setData(pItemModel, rootIndex, &rename));
    GblVariant_destruct(&rename);
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_Tree_selection(pTree), rootIndex));
    GBL_TEST_VERIFY(!GUM_Tree_expanded(pTree, rootIndex));

    // Structural invalidation clears every borrowed view handle.
    GBL_TEST_CALL(GUM_ObjectTreeModel_refresh(pModel));
    GBL_TEST_VERIFY(!GUM_ModelIndex_valid(GUM_Tree_selection(pTree)));

    const GUM_ModelIndex refreshedRoot = GUM_IItemModel_index(pItemModel,
                                                              0,
                                                              0,
                                                              GUM_MODEL_INDEX_INVALID);
    const GUM_ModelIndex refreshedFirstChild = GUM_IItemModel_index(pItemModel,
                                                                    0,
                                                                    0,
                                                                    refreshedRoot);
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(refreshedRoot));
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(refreshedFirstChild));
    GBL_TEST_VERIFY(GUM_Tree_expanded(pTree, refreshedRoot));

    GUM_Event_Mouse* pDisclosure = GUM_Event_Mouse_create();
    GBL_TEST_VERIFY(pDisclosure);
    GUM_EVENT_POINTER(pDisclosure)->position = (GUM_Vector2){ 8.0f, 12.0f };
    GUM_EVENT_INPUT(pDisclosure)->button = GUM_MOUSE_BUTTON_LEFT;
    GUM_EVENT_INPUT(pDisclosure)->state = GUM_INPUTSTATE_PRESS;
    GUM_EVENT_INPUT(pDisclosure)->action = GUM_INPUTACTION_CONFIRM;
    GBL_TEST_CALL(GUM_WIDGET_CLASSOF(pTree)->pFnInputEvent(GUM_WIDGET(pTree),
                                                           GUM_EVENT_INPUT(pDisclosure)));
    GBL_TEST_VERIFY(!GUM_Tree_expanded(pTree, refreshedRoot));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_Tree_selection(pTree), refreshedRoot));
    GBL_TEST_COMPARE(GblEvent_state(GBL_EVENT(pDisclosure)), GBL_EVENT_STATE_ACCEPTED);
    GBL_UNREF(pDisclosure);

    GBL_TEST_CALL(GUM_Tree_setExpanded(pTree, refreshedRoot, GBL_TRUE));
    GUM_Event_Mouse* pRowClick = GUM_Event_Mouse_create();
    GBL_TEST_VERIFY(pRowClick);
    GUM_EVENT_POINTER(pRowClick)->position = (GUM_Vector2){ 40.0f, 36.0f };
    GUM_EVENT_INPUT(pRowClick)->button = GUM_MOUSE_BUTTON_LEFT;
    GUM_EVENT_INPUT(pRowClick)->state = GUM_INPUTSTATE_PRESS;
    GUM_EVENT_INPUT(pRowClick)->action = GUM_INPUTACTION_CONFIRM;
    GBL_TEST_CALL(GUM_WIDGET_CLASSOF(pTree)->pFnInputEvent(GUM_WIDGET(pTree),
                                                           GUM_EVENT_INPUT(pRowClick)));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_Tree_selection(pTree), refreshedFirstChild));
    GBL_TEST_COMPARE(GblEvent_state(GBL_EVENT(pRowClick)), GBL_EVENT_STATE_ACCEPTED);
    GBL_UNREF(pRowClick);

    GBL_TEST_CALL(GUM_Tree_select(pTree, refreshedRoot));
    GBL_TEST_CALL(GUM_Tree_setExpanded(pTree, refreshedRoot, GBL_FALSE));
    GBL_TEST_VERIFY(GUM_WIDGET_CLASSOF(pTree)->pFnNavigate(GUM_WIDGET(pTree),
                                                           GUM_INPUTACTION_MOVE_RIGHT));
    GBL_TEST_VERIFY(GUM_Tree_expanded(pTree, refreshedRoot));
    GBL_TEST_VERIFY(GUM_WIDGET_CLASSOF(pTree)->pFnNavigate(GUM_WIDGET(pTree),
                                                           GUM_INPUTACTION_MOVE_DOWN));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_Tree_selection(pTree), refreshedFirstChild));
    GBL_TEST_VERIFY(GUM_WIDGET_CLASSOF(pTree)->pFnNavigate(GUM_WIDGET(pTree),
                                                           GUM_INPUTACTION_MOVE_LEFT));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_Tree_selection(pTree), refreshedRoot));

    // Right on an expanded branch is left for global focus navigation;
    // Down enters the branch instead.
    GBL_TEST_VERIFY(!GUM_WIDGET_CLASSOF(pTree)->pFnNavigate(GUM_WIDGET(pTree),
                                                            GUM_INPUTACTION_MOVE_RIGHT));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_Tree_selection(pTree), refreshedRoot));
    GBL_TEST_CALL(GUM_Tree_setExpanded(pTree, refreshedRoot, GBL_FALSE));
    GBL_TEST_VERIFY(!GUM_WIDGET_CLASSOF(pTree)->pFnNavigate(GUM_WIDGET(pTree),
                                                            GUM_INPUTACTION_MOVE_LEFT));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_Tree_selection(pTree), refreshedRoot));
    GBL_TEST_VERIFY(GUM_WIDGET_CLASSOF(pTree)->pFnNavigate(GUM_WIDGET(pTree),
                                                           GUM_INPUTACTION_MOVE_RIGHT));
    GBL_TEST_VERIFY(GUM_WIDGET_CLASSOF(pTree)->pFnNavigate(GUM_WIDGET(pTree),
                                                           GUM_INPUTACTION_MOVE_DOWN));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_Tree_selection(pTree), refreshedFirstChild));

    GBL_TEST_CALL(GUM_WIDGET_CLASSOF(pTree)->pFnUpdate(GUM_WIDGET(pTree)));
    GBL_TEST_VERIFY(GUM_Tree_scrollRange(pTree) > 0.0f);

    GUM_Event_Mouse* pWheel = GUM_Event_Mouse_create();
    GBL_TEST_VERIFY(pWheel);
    pWheel->wheel.y = -1.0f;
    GBL_TEST_CALL(GUM_WIDGET_CLASSOF(pTree)->pFnInputEvent(GUM_WIDGET(pTree),
                                                           GUM_EVENT_INPUT(pWheel)));
    GBL_TEST_COMPARE(GblEvent_state(GBL_EVENT(pWheel)), GBL_EVENT_STATE_ACCEPTED);
    GBL_UNREF(pWheel);

    GBL_TEST_CALL(GUM_Tree_setExpanded(pTree, refreshedRoot, GBL_FALSE));
    GBL_TEST_CALL(GUM_WIDGET_CLASSOF(pTree)->pFnUpdate(GUM_WIDGET(pTree)));
    GBL_TEST_COMPARE(GUM_Tree_scrollRange(pTree), 0.0f);

    GUM_PropertyModel* pPanelModel = GUM_PropertyModel_create(pRoot);
    GUM_Table* pPanelTable = GUM_Table_create("x", 350.0f,
                                              "y", 0.0f,
                                              "w", 520.0f,
                                              "h", 500.0f);
    GUM_Keyboard* pKeyboard = GUM_Keyboard_create();
    GBL_TEST_VERIFY(pPanelModel && pPanelTable && pKeyboard);
    GBL_TEST_CALL(GUM_Table_setModel(pPanelTable, GUM_IITEMMODEL(pPanelModel)));
    GBL_TEST_CALL(GUM_Tree_setExpanded(pTree, refreshedRoot, GBL_TRUE));
    GBL_TEST_CALL(GUM_Tree_select(pTree, refreshedRoot));
    GBL_TEST_CALL(GUM_Root_update(pFixture->pRoot));
    GUM_Nav_focus(GUM_INPUTDEVICE(pKeyboard), GUM_WIDGET(pTree));
    GUM_Nav_move(GUM_INPUTDEVICE(pKeyboard), GUM_INPUTACTION_MOVE_RIGHT);
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(GUM_INPUTDEVICE(pKeyboard)),
                     GUM_WIDGET(pPanelTable));

    GBL_TEST_CALL(GUM_Tree_setModel(pTree, nullptr));
    GBL_TEST_VERIFY(!GUM_Tree_model(pTree));
    GBL_TEST_VERIFY(!GUM_ModelIndex_valid(GUM_Tree_selection(pTree)));

    GUM_unref(pTree);
    GUM_unref(pKeyboard);
    GUM_unref(pPanelTable);
    GUM_unref(pPanelModel);
    GUM_IItemModel_unref(pItemModel);
    GBL_UNREF(pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(tableView)
    countersReset_();

    GUM_Widget* pWidget = GUM_Widget_create("x", 3.0f);
    GUM_Widget* pReplacement = GUM_Widget_create("x", 7.0f);
    GBL_TEST_VERIFY(pWidget && pReplacement);

    GUM_PropertyModel* pModel = GUM_PropertyModel_create(GBL_OBJECT(pWidget));
    GUM_Table* pTable = GUM_Table_create("h", 48.0f);
    GBL_TEST_VERIFY(pModel && pTable);
    GBL_TEST_VERIFY(GUM_Widget_isActive(GUM_WIDGET(pTable)));
    GBL_TEST_CALL(GUM_Table_setModel(pTable, GUM_IITEMMODEL(pModel)));

    GBL_TEST_CALL(GblSignal_connect(GBL_INSTANCE(pTable),
                                    "selectionChanged",
                                    GBL_INSTANCE(pFixture->pRoot),
                                    (GblFnPtr)selectionChanged_,
                                    nullptr));

    GUM_IItemModel* pItemModel = GUM_IITEMMODEL(pModel);
    const GUM_ModelIndex firstName = GUM_IItemModel_index(pItemModel,
                                                          0,
                                                          0,
                                                          GUM_MODEL_INDEX_INVALID);
    const GUM_ModelIndex firstValue = GUM_IItemModel_index(pItemModel,
                                                           0,
                                                           1,
                                                           GUM_MODEL_INDEX_INVALID);
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(firstName));
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(firstValue));

    GUM_Event_Mouse* pNameClick = GUM_Event_Mouse_create();
    GBL_TEST_VERIFY(pNameClick);
    GUM_EVENT_POINTER(pNameClick)->position = (GUM_Vector2){ 10.0f, 12.0f };
    GUM_EVENT_INPUT(pNameClick)->button = GUM_MOUSE_BUTTON_LEFT;
    GUM_EVENT_INPUT(pNameClick)->state = GUM_INPUTSTATE_PRESS;
    GUM_EVENT_INPUT(pNameClick)->action = GUM_INPUTACTION_CONFIRM;
    GBL_TEST_CALL(GUM_WIDGET_CLASSOF(pTable)->pFnInputEvent(GUM_WIDGET(pTable),
                                                            GUM_EVENT_INPUT(pNameClick)));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_Table_selection(pTable), firstName));
    GBL_TEST_COMPARE(GblEvent_state(GBL_EVENT(pNameClick)), GBL_EVENT_STATE_ACCEPTED);
    GBL_UNREF(pNameClick);

    GUM_Event_Mouse* pValueClick = GUM_Event_Mouse_create();
    GBL_TEST_VERIFY(pValueClick);
    GUM_EVENT_POINTER(pValueClick)->position = (GUM_Vector2){ 150.0f, 12.0f };
    GUM_EVENT_INPUT(pValueClick)->button = GUM_MOUSE_BUTTON_LEFT;
    GUM_EVENT_INPUT(pValueClick)->state = GUM_INPUTSTATE_PRESS;
    GUM_EVENT_INPUT(pValueClick)->action = GUM_INPUTACTION_CONFIRM;
    GBL_TEST_CALL(GUM_WIDGET_CLASSOF(pTable)->pFnInputEvent(GUM_WIDGET(pTable),
                                                            GUM_EVENT_INPUT(pValueClick)));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_Table_selection(pTable), firstValue));
    GBL_TEST_COMPARE(GblEvent_state(GBL_EVENT(pValueClick)), GBL_EVENT_STATE_ACCEPTED);
    GBL_UNREF(pValueClick);

    const GUM_ModelIndex xValue = findProperty_(pModel, "x", 1);
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(xValue));
    GBL_TEST_CALL(GUM_Table_select(pTable, xValue));
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_Table_selection(pTable), xValue));
    GBL_TEST_COMPARE(selectionColumn_, 1u);

    // Data changes do not invalidate cell selection.
    GBL_VARIANT(value);
    GBL_TEST_CALL(GblVariant_setFloat(&value, 9.0f));
    GBL_TEST_CALL(GUM_IItemModel_setData(GUM_IITEMMODEL(pModel), xValue, &value));
    GblVariant_destruct(&value);
    GBL_TEST_VERIFY(GUM_ModelIndex_equal(GUM_Table_selection(pTable), xValue));

    GBL_TEST_VERIFY(GUM_WIDGET_CLASSOF(pTable)->pFnNavigate(GUM_WIDGET(pTable),
                                                            GUM_INPUTACTION_MOVE_LEFT));
    GBL_TEST_COMPARE(GUM_Table_selection(pTable).column, 0u);
    GBL_TEST_VERIFY(GUM_WIDGET_CLASSOF(pTable)->pFnNavigate(GUM_WIDGET(pTable),
                                                            GUM_INPUTACTION_MOVE_RIGHT));
    GBL_TEST_COMPARE(GUM_Table_selection(pTable).column, 1u);

    GBL_TEST_CALL(GUM_WIDGET_CLASSOF(pTable)->pFnUpdate(GUM_WIDGET(pTable)));
    GBL_TEST_VERIFY(GUM_Table_scrollRange(pTable) > 0.0f);

    GUM_Event_Mouse* pWheel = GUM_Event_Mouse_create();
    GBL_TEST_VERIFY(pWheel);
    pWheel->wheel.y = -1.0f;
    GBL_TEST_CALL(GUM_WIDGET_CLASSOF(pTable)->pFnInputEvent(GUM_WIDGET(pTable),
                                                            GUM_EVENT_INPUT(pWheel)));
    GBL_TEST_COMPARE(GblEvent_state(GBL_EVENT(pWheel)), GBL_EVENT_STATE_ACCEPTED);
    GBL_UNREF(pWheel);

    // PropertyModel replacement emits structureChanged and clears Table selection.
    GBL_TEST_CALL(GUM_PropertyModel_setObject(pModel, GBL_OBJECT(pReplacement)));
    GBL_TEST_VERIFY(!GUM_ModelIndex_valid(GUM_Table_selection(pTable)));

    GBL_TEST_CALL(GUM_Table_setModel(pTable, nullptr));
    GBL_TEST_VERIFY(!GUM_Table_model(pTable));
    GBL_TEST_CALL(GUM_WIDGET_CLASSOF(pTable)->pFnUpdate(GUM_WIDGET(pTable)));
    GBL_TEST_COMPARE(GUM_Table_scrollRange(pTable), 0.0f);

    GUM_unref(pTable);
    GUM_IItemModel_unref(GUM_IITEMMODEL(pModel));
    GUM_unref(pWidget);
    GUM_unref(pReplacement);
GBL_TEST_CASE_END

GBL_TEST_CASE(largeHierarchy)
    countersReset_();

    GUM_LargeHierarchyModel* pModel = GUM_LargeHierarchyModel_create_();
    GUM_Tree* pTree = GUM_Tree_create("h", 120.0f, "rowHeight", 20.0f);
    GBL_TEST_VERIFY(pModel && pTree);

    GUM_IItemModel* pItemModel = GUM_IITEMMODEL(pModel);
    const GUM_ModelIndex rootIndex = GUM_IItemModel_index(pItemModel,
                                                          0,
                                                          0,
                                                          GUM_MODEL_INDEX_INVALID);
    GBL_TEST_VERIFY(GUM_ModelIndex_valid(rootIndex));
    GBL_TEST_COMPARE(GUM_IItemModel_rowCount(pItemModel, rootIndex),
                     (size_t)GUM_LARGE_HIERARCHY_CHILD_COUNT_);

    GBL_TEST_CALL(GUM_Tree_setModel(pTree, pItemModel));
    GBL_TEST_CALL(GUM_WIDGET_CLASSOF(pTree)->pFnUpdate(GUM_WIDGET(pTree)));
    GBL_TEST_VERIFY(GUM_Tree_scrollRange(pTree) > 20000.0f);

    // Keep the model attached so Tree destruction exercises its live-model teardown path.
    GUM_unref(pTree);
    GUM_IItemModel_unref(pItemModel);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(objectTreeModel,
                  propertyModel,
                  propertyOverride,
                  treeView,
                  tableView,
                  largeHierarchy)
