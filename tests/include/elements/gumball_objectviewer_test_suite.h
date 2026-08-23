#ifndef GUMBALL_OBJECTVIEWER_TEST_SUITE_H
#define GUMBALL_OBJECTVIEWER_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

#define GUM_OBJECTVIEWER_TEST_SUITE_TYPE (GBL_TYPEID(GUM_ObjectViewerTestSuite))

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_ObjectViewerTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_ObjectViewerTestSuite, GblTestSuite)

GblType GUM_ObjectViewerTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_OBJECTVIEWER_TEST_SUITE_H
