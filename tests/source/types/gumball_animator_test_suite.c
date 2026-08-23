#include "types/gumball_animator_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/meta/signals/gimbal_c_closure.h>
#include <gumball/types/gumball_animator.h>

#define GBL_SELF_TYPE GUM_AnimatorTestSuite

GBL_TEST_FIXTURE_NONE
GBL_TEST_INIT_NONE
GBL_TEST_FINAL_NONE

static void noop_(void) {}

GBL_TEST_CASE(make)
    GUM_Animator animator = GUM_Animator_make(3.0f, 1.0f, GUM_EASE_LINEAR);

    GBL_TEST_COMPARE(animator.from, 3.0f);
    GBL_TEST_COMPARE(animator.to, 3.0f);
    GBL_TEST_COMPARE(animator.current, 3.0f);
    GBL_TEST_VERIFY(GUM_Animator_settled(&animator));
GBL_TEST_CASE_END

GBL_TEST_CASE(set)
    GUM_Animator animator = GUM_Animator_make(3.0f, 1.0f, GUM_EASE_LINEAR);
    GUM_Animator_set(&animator, 7.0f);

    GBL_TEST_COMPARE(animator.from, 3.0f);
    GBL_TEST_COMPARE(animator.to, 7.0f);
    GBL_TEST_COMPARE(animator.elapsed, 0.0f);
    GBL_TEST_VERIFY(!GUM_Animator_settled(&animator));

    animator = GUM_Animator_make(2.0f, 0.0f, GUM_EASE_LINEAR);
    GUM_Animator_set(&animator, 9.0f);
    GBL_TEST_COMPARE(animator.current, 9.0f);
    GBL_TEST_VERIFY(GUM_Animator_settled(&animator));
GBL_TEST_CASE_END

GBL_TEST_CASE(update)
    GUM_Animator animator = GUM_Animator_make(0.0f, 1.0f, GUM_EASE_LINEAR);
    GUM_Animator_set(&animator, 10.0f);

    GBL_TEST_VERIFY(!GUM_Animator_update(&animator, 0.0f));
    GBL_TEST_VERIFY(!GUM_Animator_update(&animator, -0.5f));
    GBL_TEST_COMPARE(animator.current, 0.0f);
    GBL_TEST_COMPARE(animator.elapsed, 0.0f);

    GBL_TEST_VERIFY(GUM_Animator_update(&animator, 0.5f));
    GBL_TEST_COMPARE(animator.current, 5.0f);
    GBL_TEST_COMPARE(animator.elapsed, 0.5f);

    GBL_TEST_VERIFY(GUM_Animator_update(&animator, 0.5f));
    GBL_TEST_COMPARE(animator.current, 10.0f);
    GBL_TEST_VERIFY(GUM_Animator_settled(&animator));
    GBL_TEST_VERIFY(!GUM_Animator_update(&animator, 1.0f));
GBL_TEST_CASE_END

GBL_TEST_CASE(easing)
    GUM_Animator animator = GUM_Animator_make(0.0f, 1.0f, (GUM_EasingType)GUM_EASE_COUNT);
    GUM_Animator_set(&animator, 10.0f);
    GBL_TEST_COMPARE(animator.easing, GUM_EASE_LINEAR);
    GBL_TEST_VERIFY(GUM_Animator_update(&animator, 0.5f));
    GBL_TEST_COMPARE(animator.current, 5.0f);

    animator = GUM_Animator_makeCustom(0.0f, 1.0f, nullptr);
    GUM_Animator_set(&animator, 10.0f);
    GBL_TEST_COMPARE(animator.easing, GUM_EASE_LINEAR);
    GBL_TEST_VERIFY(GUM_Animator_update(&animator, 0.5f));
    GBL_TEST_COMPARE(animator.current, 5.0f);
GBL_TEST_CASE_END

GBL_TEST_CASE(onDone)
    GUM_Animator animator = GUM_Animator_make(0.0f, 1.0f, GUM_EASE_LINEAR);
    GblClosure* pClosure = GBL_CLOSURE(GblCClosure_create((GblFnPtr)noop_, nullptr));
    GBL_TEST_VERIFY(pClosure);

    GUM_Animator_setOnDone(&animator, pClosure);
    GblClosure_unref(pClosure);

    pClosure = animator.pOnDone;
    GBL_TEST_VERIFY(pClosure);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pClosure)), 1u);

    GUM_Animator_setOnDone(&animator, pClosure);
    GBL_TEST_COMPARE(animator.pOnDone, pClosure);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pClosure)), 1u);

    GUM_Animator_setOnDone(&animator, nullptr);
    GBL_TEST_VERIFY(!animator.pOnDone);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(make,
                  set,
                  update,
                  easing,
                  onDone)
