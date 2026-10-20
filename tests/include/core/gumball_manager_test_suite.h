#ifndef GUMBALL_MANAGER_TEST_SUITE_H
#define GUMBALL_MANAGER_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_MANAGER_TEST_SUITE_TYPE          (GBL_TYPEID     (GUM_ManagerTestSuite))           //!< Returns the GUM_ManagerTestSuite Type UUID
#define GUM_MANAGER_TEST_SUITE(self)         (GBL_CAST       (GUM_ManagerTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_ManagerTestSuite
#define GUM_MANAGER_TEST_SUITE_CLASS(klass)  (GBL_CLASS_CAST (GUM_ManagerTestSuite, klass))    //!< Casts a class     of a compatible test suite to a GUM_ManagerTestSuiteClass
#define GUM_MANAGER_TEST_SUITE_CLASSOF(self) (GBL_CLASSOF    (GUM_ManagerTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_ManagerTestSuiteClass
//! @}

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_ManagerTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_ManagerTestSuite, GblTestSuite)

GblType GUM_ManagerTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_MANAGER_TEST_SUITE_H
