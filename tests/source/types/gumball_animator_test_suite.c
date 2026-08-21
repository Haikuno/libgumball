#include "types/gumball_animator_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/types/gumball_animator.h>

#define GBL_SELF_TYPE GUM_AnimatorTestSuite

GBL_TEST_FIXTURE {
    GUM_Animator animator;
};

GBL_TEST_INIT_NONE
GBL_TEST_FINAL_NONE

GBL_TEST_CASE(makeSettled)
    pFixture->animator = GUM_Animator_make(3.0f, 1.0f, GUM_EASE_LINEAR);

    GBL_TEST_COMPARE(pFixture->animator.from, 3.0f);
    GBL_TEST_COMPARE(pFixture->animator.to, 3.0f);
    GBL_TEST_COMPARE(pFixture->animator.current, 3.0f);
    GBL_TEST_VERIFY(GUM_Animator_settled(&pFixture->animator));
GBL_TEST_CASE_END

GBL_TEST_CASE(retarget)
    GUM_Animator_set(&pFixture->animator, 7.0f);

    GBL_TEST_COMPARE(pFixture->animator.from, 3.0f);
    GBL_TEST_COMPARE(pFixture->animator.to, 7.0f);
    GBL_TEST_COMPARE(pFixture->animator.elapsed, 0.0f);
    GBL_TEST_VERIFY(!GUM_Animator_settled(&pFixture->animator));
GBL_TEST_CASE_END

GBL_TEST_CASE(updateHalf)
    GBL_TEST_VERIFY(GUM_Animator_update(&pFixture->animator, 0.5f));
    GBL_TEST_COMPARE(pFixture->animator.current, 5.0f);
    GBL_TEST_COMPARE(pFixture->animator.elapsed, 0.5f);
GBL_TEST_CASE_END

GBL_TEST_CASE(updateDone)
    GBL_TEST_VERIFY(GUM_Animator_update(&pFixture->animator, 0.5f));
    GBL_TEST_COMPARE(pFixture->animator.current, 7.0f);
    GBL_TEST_VERIFY(GUM_Animator_settled(&pFixture->animator));
    GBL_TEST_VERIFY(!GUM_Animator_update(&pFixture->animator, 1.0f));
GBL_TEST_CASE_END

GBL_TEST_CASE(zeroDuration)
    GUM_Animator animator = GUM_Animator_make(2.0f, 0.0f, GUM_EASE_LINEAR);
    GUM_Animator_set(&animator, 9.0f);

    GBL_TEST_COMPARE(animator.current, 9.0f);
    GBL_TEST_VERIFY(GUM_Animator_settled(&animator));
GBL_TEST_CASE_END

GBL_TEST_REGISTER(makeSettled,
                  retarget,
                  updateHalf,
                  updateDone,
                  zeroDuration)
