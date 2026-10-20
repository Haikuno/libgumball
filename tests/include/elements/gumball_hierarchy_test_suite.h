#ifndef GUMBALL_HIERARCHY_TEST_SUITE_H
#define GUMBALL_HIERARCHY_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_HIERARCHY_TEST_SUITE_TYPE          (GBL_TYPEID     (GUM_HierarchyTestSuite))           //!< Returns the GUM_HierarchyTestSuite Type UUID
#define GUM_HIERARCHY_TEST_SUITE(self)         (GBL_CAST       (GUM_HierarchyTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_HierarchyTestSuite
#define GUM_HIERARCHY_TEST_SUITE_CLASS(klass)  (GBL_CLASS_CAST (GUM_HierarchyTestSuite, klass))    //!< Casts a class     of a compatible test suite to a GUM_HierarchyTestSuiteClass
#define GUM_HIERARCHY_TEST_SUITE_CLASSOF(self) (GBL_CLASSOF    (GUM_HierarchyTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_HierarchyTestSuiteClass
//! @}

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_HierarchyTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_HierarchyTestSuite, GblTestSuite)

GblType GUM_HierarchyTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_HIERARCHY_TEST_SUITE_H
