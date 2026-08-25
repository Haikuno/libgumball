#include "types/gumball_vector2_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/types/gumball_vector2.h>
#include <math.h>

#define GBL_SELF_TYPE GUM_Vector2TestSuite

GBL_TEST_FIXTURE_NONE
GBL_TEST_INIT_NONE
GBL_TEST_FINAL_NONE

GBL_TEST_CASE(length)
    const GUM_Vector2 value = { 3.0e30f, 4.0e30f };
    const float length = GUM_Vector2_length(value);
    const float distance = GUM_Vector2_distance((GUM_Vector2){ 0.0f, 0.0f }, value);

    GBL_TEST_VERIFY(isfinite(length));
    GBL_TEST_VERIFY(isfinite(distance));
    GBL_TEST_VERIFY(fabsf(length - 5.0e30f) / 5.0e30f < 1.0e-6f);
    GBL_TEST_VERIFY(fabsf(distance - 5.0e30f) / 5.0e30f < 1.0e-6f);
GBL_TEST_CASE_END

GBL_TEST_CASE(angle)
    const GUM_Vector2 from = { 0.0f, 0.0f };
    const GUM_Vector2 to   = { -488201.28f, 448340.22f };
    const GUM_Vector2 dir  = { -488198.3f,  448337.5f };

    float angle = GUM_Vector2_angleTo(from, to, dir);
    GBL_TEST_VERIFY(isfinite(angle));
    GBL_TEST_VERIFY(angle >= 0.0f);
    GBL_TEST_VERIFY(angle < 0.001f);

    const GUM_Vector2 parallel = { 1.0e30f, 1.0e30f };
    const GUM_Vector2 quarterTurn = { -1.0e30f, 1.0e30f };

    angle = GUM_Vector2_angleTo(from, parallel, parallel);
    GBL_TEST_VERIFY(isfinite(angle));
    GBL_TEST_VERIFY(angle < 0.001f);

    angle = GUM_Vector2_signedAngleTo(from, quarterTurn, parallel);
    GBL_TEST_VERIFY(isfinite(angle));
    GBL_TEST_VERIFY(fabsf(angle - GBL_F_PI / 2.0f) < 1.0e-5f);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(length,
                  angle)
