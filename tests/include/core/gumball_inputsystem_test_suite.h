#ifndef GUMBALL_INPUTSYSTEM_TEST_SUITE_H
#define GUMBALL_INPUTSYSTEM_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GUM_INPUTSYSTEM_TEST_SUITE_TYPE (GBL_TYPEID(GUM_InputSystemTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_InputSystemTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_InputSystemTestSuite, GblTestSuite)

GblType GUM_InputSystemTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_INPUTSYSTEM_TEST_SUITE_H
