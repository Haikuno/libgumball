#ifndef GUMBALL_MANAGER_TEST_SUITE_H
#define GUMBALL_MANAGER_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GUM_MANAGER_TEST_SUITE_TYPE (GBL_TYPEID(GUM_ManagerTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_ManagerTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_ManagerTestSuite, GblTestSuite)

GblType GUM_ManagerTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_MANAGER_TEST_SUITE_H
