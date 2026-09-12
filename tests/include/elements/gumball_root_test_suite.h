#ifndef GUMBALL_ROOT_TEST_SUITE_H
#define GUMBALL_ROOT_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GUM_ROOT_TEST_SUITE_TYPE (GBL_TYPEID(GUM_RootTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_RootTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_RootTestSuite, GblTestSuite)

GblType GUM_RootTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_ROOT_TEST_SUITE_H
