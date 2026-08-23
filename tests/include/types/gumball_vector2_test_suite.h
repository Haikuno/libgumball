#ifndef GUMBALL_VECTOR2_TEST_SUITE_H
#define GUMBALL_VECTOR2_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GUM_VECTOR2_TEST_SUITE_TYPE (GBL_TYPEID(GUM_Vector2TestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_Vector2TestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_Vector2TestSuite, GblTestSuite)

GblType GUM_Vector2TestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_VECTOR2_TEST_SUITE_H
