#ifndef GUMBALL_NAVIGATION_TEST_SUITE_H
#define GUMBALL_NAVIGATION_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GUM_NAVIGATION_TEST_SUITE_TYPE (GBL_TYPEID(GUM_NavigationTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_NavigationTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_NavigationTestSuite, GblTestSuite)

GblType GUM_NavigationTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_NAVIGATION_TEST_SUITE_H
