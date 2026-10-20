#ifndef GUMBALL_MODELVIEW_TEST_SUITE_H
#define GUMBALL_MODELVIEW_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_MODELVIEW_TEST_SUITE_TYPE          (GBL_TYPEID     (GUM_ModelViewTestSuite))           //!< Returns the GUM_ModelViewTestSuite Type UUID
#define GUM_MODELVIEW_TEST_SUITE(self)         (GBL_CAST       (GUM_ModelViewTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_ModelViewTestSuite
#define GUM_MODELVIEW_TEST_SUITE_CLASS(klass)  (GBL_CLASS_CAST (GUM_ModelViewTestSuite, klass))    //!< Casts a class     of a compatible test suite to a GUM_ModelViewTestSuiteClass
#define GUM_MODELVIEW_TEST_SUITE_CLASSOF(self) (GBL_CLASSOF    (GUM_ModelViewTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_ModelViewTestSuiteClass
//! @}

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_ModelViewTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_ModelViewTestSuite, GblTestSuite)

GblType GUM_ModelViewTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_MODELVIEW_TEST_SUITE_H
