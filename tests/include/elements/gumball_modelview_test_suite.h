#ifndef GUMBALL_MODELVIEW_TEST_SUITE_H
#define GUMBALL_MODELVIEW_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GUM_MODELVIEW_TEST_SUITE_TYPE (GBL_TYPEID(GUM_ModelViewTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_ModelViewTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_ModelViewTestSuite, GblTestSuite)

GblType GUM_ModelViewTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_MODELVIEW_TEST_SUITE_H
