#ifndef GUMBALL_RECTANGLE_TEST_SUITE_H
#define GUMBALL_RECTANGLE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GUM_RECTANGLE_TEST_SUITE_TYPE (GBL_TYPEID(GUM_RectangleTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_RectangleTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_RectangleTestSuite, GblTestSuite)

GblType GUM_RectangleTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_RECTANGLE_TEST_SUITE_H
