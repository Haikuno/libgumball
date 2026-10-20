#ifndef GUMBALL_COMMON_TEST_SUITE_H
#define GUMBALL_COMMON_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_COMMON_TEST_SUITE_TYPE          (GBL_TYPEID     (GUM_CommonTestSuite))           //!< Returns the GUM_CommonTestSuite Type UUID
#define GUM_COMMON_TEST_SUITE(self)         (GBL_CAST       (GUM_CommonTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_CommonTestSuite
#define GUM_COMMON_TEST_SUITE_CLASS(klass)  (GBL_CLASS_CAST (GUM_CommonTestSuite, klass))    //!< Casts a class     of a compatible test suite to a GUM_CommonTestSuiteClass
#define GUM_COMMON_TEST_SUITE_CLASSOF(self) (GBL_CLASSOF    (GUM_CommonTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_CommonTestSuiteClass
//! @}

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_CommonTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_CommonTestSuite, GblTestSuite)

GblType GUM_CommonTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_COMMON_TEST_SUITE_H
