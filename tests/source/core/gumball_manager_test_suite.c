#include "core/gumball_manager_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#define GBL_SELF_TYPE GUM_ManagerTestSuite

GBL_TEST_FIXTURE_NONE
GBL_TEST_INIT_NONE
GBL_TEST_FINAL_NONE

GBL_TEST_CASE(invalidResources)
    GUM_Root* pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pRoot);

    GUM_IResource* pInvalidFont = GUM_Manager_load("invalid.ttf");
    GUM_IResource* pInvalidTexture = GUM_Manager_load("invalid.png");

    GBL_TEST_COMPARE(pInvalidFont, nullptr);
    GBL_TEST_COMPARE(pInvalidTexture, nullptr);

    GUM_unref(pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(rootShutdownReleasesResources)
    GUM_Root* pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pRoot);

    GUM_IResource* pTexture = GUM_Manager_load("koslogo.png");
    GBL_TEST_VERIFY(pTexture);

    GUM_Font* pFont = GUM_FONT(GblBox_create(GUM_FONT_TYPE));
    GBL_TEST_VERIFY(pFont);
    GUM_Font_setDefault(pFont);

    GUM_unref(pRoot);

    GBL_TEST_COMPARE(GUM_Font_default(), nullptr);
    GBL_TEST_COMPARE(GUM_IResource_data(pTexture), nullptr);

    GUM_IResource_unref(pTexture);
    GUM_IResource_unref(GUM_IRESOURCE(pFont));
GBL_TEST_CASE_END

GBL_TEST_CASE(rootRestart)
    GUM_Root* pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pRoot);

    GUM_IResource* pTexture = GUM_Manager_load("koslogo.png");
    GBL_TEST_VERIFY(pTexture);

    GUM_Manager_unload(pTexture);
    GBL_TEST_COMPARE(GUM_IResource_data(pTexture), nullptr);

    GUM_IResource_unref(pTexture);
    GUM_unref(pRoot);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(invalidResources,
                  rootShutdownReleasesResources,
                  rootRestart)
