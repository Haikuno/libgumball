#ifndef GUMBALL_NAVIGATION_TEST_SUITE_H
#define GUMBALL_NAVIGATION_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_NAVIGATION_TEST_SUITE_TYPE          (GBL_TYPEID     (GUM_NavigationTestSuite))           //!< Returns the GUM_NavigationTestSuite Type UUID
#define GUM_NAVIGATION_TEST_SUITE(self)         (GBL_CAST       (GUM_NavigationTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_NavigationTestSuite
#define GUM_NAVIGATION_TEST_SUITE_CLASS(klass)  (GBL_CLASS_CAST (GUM_NavigationTestSuite, klass))    //!< Casts a class     of a compatible test suite to a GUM_NavigationTestSuiteClass
#define GUM_NAVIGATION_TEST_SUITE_CLASSOF(self) (GBL_CLASSOF    (GUM_NavigationTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_NavigationTestSuiteClass
//! @}

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_NavigationTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_NavigationTestSuite, GblTestSuite)

GblType GUM_NavigationTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_NAVIGATION_TEST_SUITE_H
