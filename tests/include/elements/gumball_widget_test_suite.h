#ifndef GUMBALL_WIDGET_TEST_SUITE_H
#define GUMBALL_WIDGET_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_WIDGET_TEST_SUITE_TYPE          (GBL_TYPEID     (GUM_WidgetTestSuite))           //!< Returns the GUM_WidgetTestSuite Type UUID
#define GUM_WIDGET_TEST_SUITE(self)         (GBL_CAST       (GUM_WidgetTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_WidgetTestSuite
#define GUM_WIDGET_TEST_SUITE_CLASS(klass)  (GBL_CLASS_CAST (GUM_WidgetTestSuite, klass))    //!< Casts a class     of a compatible test suite to a GUM_WidgetTestSuiteClass
#define GUM_WIDGET_TEST_SUITE_CLASSOF(self) (GBL_CLASSOF    (GUM_WidgetTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_WidgetTestSuiteClass
//! @}

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_WidgetTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_WidgetTestSuite, GblTestSuite)

GblType GUM_WidgetTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_WIDGET_TEST_SUITE_H
