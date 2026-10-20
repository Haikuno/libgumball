#ifndef GUMBALL_INPUTSYSTEM_TEST_SUITE_H
#define GUMBALL_INPUTSYSTEM_TEST_SUITE_H

#include <gimbal/test/gimbal_test_suite.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_INPUTSYSTEM_TEST_SUITE_TYPE          (GBL_TYPEID     (GUM_InputSystemTestSuite))           //!< Returns the GUM_InputSystemTestSuite Type UUID
#define GUM_INPUTSYSTEM_TEST_SUITE(self)         (GBL_CAST       (GUM_InputSystemTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_InputSystemTestSuite
#define GUM_INPUTSYSTEM_TEST_SUITE_CLASS(klass)  (GBL_CLASS_CAST (GUM_InputSystemTestSuite, klass))    //!< Casts a class     of a compatible test suite to a GUM_InputSystemTestSuiteClass
#define GUM_INPUTSYSTEM_TEST_SUITE_CLASSOF(self) (GBL_CLASSOF    (GUM_InputSystemTestSuite, self))     //!< Casts an instance of a compatible test suite to a GUM_InputSystemTestSuiteClass
//! @}

GBL_DECLS_BEGIN

GBL_CLASS_DERIVE_EMPTY(GUM_InputSystemTestSuite, GblTestSuite)
GBL_INSTANCE_DERIVE_EMPTY(GUM_InputSystemTestSuite, GblTestSuite)

GblType GUM_InputSystemTestSuite_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUMBALL_INPUTSYSTEM_TEST_SUITE_H
