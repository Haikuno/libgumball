#ifndef GUMBALL_ROOT_TEST_SUITE_H
#define GUMBALL_ROOT_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_ROOT_TEST_SUITE_TYPE          (GBL_TYPEID     (GUM_RootTestSuite))           //!< Returns the GUM_RootTestSuite Type UUID
#define GUM_ROOT_TEST_SUITE(self)         (GBL_CAST       (GUM_RootTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_RootTestSuite
#define GUM_ROOT_TEST_SUITE_CLASS(klass)  (GBL_CLASS_CAST (GUM_RootTestSuite, klass))    //!< Casts a class     of a compatible test suite to a GUM_RootTestSuiteClass
#define GUM_ROOT_TEST_SUITE_CLASSOF(self) (GBL_CLASSOF    (GUM_RootTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_RootTestSuiteClass
//! @}

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_RootTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_RootTestSuite, GblTestSuite)

GblType GUM_RootTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_ROOT_TEST_SUITE_H
