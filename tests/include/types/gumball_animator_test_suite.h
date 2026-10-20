#ifndef GUMBALL_ANIMATOR_TEST_SUITE_H
#define GUMBALL_ANIMATOR_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_ANIMATOR_TEST_SUITE_TYPE          (GBL_TYPEID     (GUM_AnimatorTestSuite))           //!< Returns the GUM_AnimatorTestSuite Type UUID
#define GUM_ANIMATOR_TEST_SUITE(self)         (GBL_CAST       (GUM_AnimatorTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_AnimatorTestSuite
#define GUM_ANIMATOR_TEST_SUITE_CLASS(klass)  (GBL_CLASS_CAST (GUM_AnimatorTestSuite, klass))    //!< Casts a class     of a compatible test suite to a GUM_AnimatorTestSuiteClass
#define GUM_ANIMATOR_TEST_SUITE_CLASSOF(self) (GBL_CLASSOF    (GUM_AnimatorTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_AnimatorTestSuiteClass
//! @}

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_AnimatorTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_AnimatorTestSuite, GblTestSuite)

GblType GUM_AnimatorTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_ANIMATOR_TEST_SUITE_H
