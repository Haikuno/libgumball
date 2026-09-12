#ifndef GUMBALL_ANIMATOR_TEST_SUITE_H
#define GUMBALL_ANIMATOR_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GUM_ANIMATOR_TEST_SUITE_TYPE (GBL_TYPEID(GUM_AnimatorTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_AnimatorTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_AnimatorTestSuite, GblTestSuite)

GblType GUM_AnimatorTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_ANIMATOR_TEST_SUITE_H
