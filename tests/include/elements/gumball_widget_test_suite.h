#ifndef GUMBALL_WIDGET_TEST_SUITE_H
#define GUMBALL_WIDGET_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GUM_WIDGET_TEST_SUITE_TYPE (GBL_TYPEID(GUM_WidgetTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_WidgetTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_WidgetTestSuite, GblTestSuite)

GblType GUM_WidgetTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_WIDGET_TEST_SUITE_H
