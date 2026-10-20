#ifndef GUMBALL_RECTANGLE_TEST_SUITE_H
#define GUMBALL_RECTANGLE_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_RECTANGLE_TEST_SUITE_TYPE          (GBL_TYPEID     (GUM_RectangleTestSuite))           //!< Returns the GUM_RectangleTestSuite Type UUID
#define GUM_RECTANGLE_TEST_SUITE(self)         (GBL_CAST       (GUM_RectangleTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_RectangleTestSuite
#define GUM_RECTANGLE_TEST_SUITE_CLASS(klass)  (GBL_CLASS_CAST (GUM_RectangleTestSuite, klass))    //!< Casts a class     of a compatible test suite to a GUM_RectangleTestSuiteClass
#define GUM_RECTANGLE_TEST_SUITE_CLASSOF(self) (GBL_CLASSOF    (GUM_RectangleTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_RectangleTestSuiteClass
//! @}

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_RectangleTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_RectangleTestSuite, GblTestSuite)

GblType GUM_RectangleTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_RECTANGLE_TEST_SUITE_H
