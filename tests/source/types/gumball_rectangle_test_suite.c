#include "types/gumball_rectangle_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/types/gumball_rectangle.h>

#define GBL_SELF_TYPE GUM_RectangleTestSuite

GBL_TEST_FIXTURE_NONE
GBL_TEST_INIT_NONE
GBL_TEST_FINAL_NONE

GBL_TEST_CASE(intersect)
    GUM_Rectangle result = GUM_Rectangle_intersect((GUM_Rectangle){ 0, 0, 10, 10 },
                                                   (GUM_Rectangle){ 5, 5, 10, 10 });
    GBL_TEST_COMPARE(result.x, 5.0f);
    GBL_TEST_COMPARE(result.y, 5.0f);
    GBL_TEST_COMPARE(result.width, 5.0f);
    GBL_TEST_COMPARE(result.height, 5.0f);

    result = GUM_Rectangle_intersect((GUM_Rectangle){ 0, 0, 20, 20 },
                                     (GUM_Rectangle){ 4, 6, 8, 5 });
    GBL_TEST_COMPARE(result.x, 4.0f);
    GBL_TEST_COMPARE(result.y, 6.0f);
    GBL_TEST_COMPARE(result.width, 8.0f);
    GBL_TEST_COMPARE(result.height, 5.0f);

    result = GUM_Rectangle_intersect((GUM_Rectangle){ 0, 0, 10, 10 },
                                     (GUM_Rectangle){ 20, 30, 4, 5 });
    GBL_TEST_COMPARE(result.width, 0.0f);
    GBL_TEST_COMPARE(result.height, 0.0f);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(intersect)
