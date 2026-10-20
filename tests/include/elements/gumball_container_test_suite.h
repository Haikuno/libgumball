#ifndef GUMBALL_CONTAINER_TEST_SUITE_H
#define GUMBALL_CONTAINER_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_CONTAINER_TEST_SUITE_TYPE          (GBL_TYPEID     (GUM_ContainerTestSuite))           //!< Returns the GUM_ContainerTestSuite Type UUID
#define GUM_CONTAINER_TEST_SUITE(self)         (GBL_CAST       (GUM_ContainerTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_ContainerTestSuite
#define GUM_CONTAINER_TEST_SUITE_CLASS(klass)  (GBL_CLASS_CAST (GUM_ContainerTestSuite, klass))    //!< Casts a class     of a compatible test suite to a GUM_ContainerTestSuiteClass
#define GUM_CONTAINER_TEST_SUITE_CLASSOF(self) (GBL_CLASSOF    (GUM_ContainerTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_ContainerTestSuiteClass
//! @}

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_ContainerTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_ContainerTestSuite, GblTestSuite)

GblType GUM_ContainerTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_CONTAINER_TEST_SUITE_H
