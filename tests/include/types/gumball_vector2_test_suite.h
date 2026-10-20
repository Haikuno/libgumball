#ifndef GUMBALL_VECTOR2_TEST_SUITE_H
#define GUMBALL_VECTOR2_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_VECTOR2_TEST_SUITE_TYPE          (GBL_TYPEID     (GUM_Vector2TestSuite))           //!< Returns the GUM_Vector2TestSuite Type UUID
#define GUM_VECTOR2_TEST_SUITE(self)         (GBL_CAST       (GUM_Vector2TestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_Vector2TestSuite
#define GUM_VECTOR2_TEST_SUITE_CLASS(klass)  (GBL_CLASS_CAST (GUM_Vector2TestSuite, klass))    //!< Casts a class     of a compatible test suite to a GUM_Vector2TestSuiteClass
#define GUM_VECTOR2_TEST_SUITE_CLASSOF(self) (GBL_CLASSOF    (GUM_Vector2TestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_Vector2TestSuiteClass
//! @}

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_Vector2TestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_Vector2TestSuite, GblTestSuite)

GblType GUM_Vector2TestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_VECTOR2_TEST_SUITE_H
