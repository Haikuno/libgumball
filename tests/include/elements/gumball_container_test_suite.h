#ifndef GUMBALL_CONTAINER_TEST_SUITE_H
#define GUMBALL_CONTAINER_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GUM_CONTAINER_TEST_SUITE_TYPE (GBL_TYPEID(GUM_ContainerTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_ContainerTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_ContainerTestSuite, GblTestSuite)

GblType GUM_ContainerTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_CONTAINER_TEST_SUITE_H
