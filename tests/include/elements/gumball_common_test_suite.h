#ifndef GUMBALL_COMMON_TEST_SUITE_H
#define GUMBALL_COMMON_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GUM_COMMON_TEST_SUITE_TYPE (GBL_TYPEID(GUM_CommonTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_CommonTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_CommonTestSuite, GblTestSuite)

GblType GUM_CommonTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_COMMON_TEST_SUITE_H
