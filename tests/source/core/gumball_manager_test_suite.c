#include "core/gumball_manager_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#define GBL_SELF_TYPE GUM_ManagerTestSuite

GBL_TEST_FIXTURE_NONE
GBL_TEST_INIT_NONE
GBL_TEST_FINAL_NONE

GBL_TEST_CASE(invalidResources)
    GUM_Root* pRoot = GUM_Root_create();
    GUM_IResource* pInvalidFont = pRoot ? GUM_Manager_load("invalid.ttf") : nullptr;
    GUM_IResource* pInvalidTexture = pRoot ? GUM_Manager_load("invalid.png") : nullptr;

    if (pRoot)
        GUM_unref(pRoot);

    GBL_TEST_VERIFY(pRoot);
    GBL_TEST_COMPARE(pInvalidFont, nullptr);
    GBL_TEST_COMPARE(pInvalidTexture, nullptr);
GBL_TEST_CASE_END

GBL_TEST_CASE(rootShutdownReleasesResources)
    GUM_Root* pRoot = GUM_Root_create();
    GUM_IResource* pTexture = pRoot ? GUM_Manager_load("koslogo.png") : nullptr;
    GUM_Font* pFont = pRoot ? GUM_FONT(GblBox_create(GUM_FONT_TYPE)) : nullptr;

    if (pFont)
        GUM_Font_setDefault(pFont);
    if (pRoot)
        GUM_unref(pRoot);

    const bool defaultCleared = GUM_Font_default() == nullptr;
    const bool textureReleased = pTexture && GUM_IResource_data(pTexture) == nullptr;

    if (pTexture)
        GUM_IResource_unref(pTexture);
    if (pFont)
        GUM_IResource_unref(GUM_IRESOURCE(pFont));

    GBL_TEST_VERIFY(pRoot);
    GBL_TEST_VERIFY(pTexture);
    GBL_TEST_VERIFY(pFont);
    GBL_TEST_VERIFY(defaultCleared);
    GBL_TEST_VERIFY(textureReleased);
GBL_TEST_CASE_END

GBL_TEST_CASE(rootRestart)
    GUM_Root* pRoot = GUM_Root_create();
    GUM_IResource* pTexture = pRoot ? GUM_Manager_load("koslogo.png") : nullptr;
    bool unloaded = false;

    if (pTexture) {
        GUM_Manager_unload(pTexture);
        unloaded = GUM_IResource_data(pTexture) == nullptr;
        GUM_IResource_unref(pTexture);
    }
    if (pRoot)
        GUM_unref(pRoot);

    GBL_TEST_VERIFY(pRoot);
    GBL_TEST_VERIFY(pTexture);
    GBL_TEST_VERIFY(unloaded);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(invalidResources,
                  rootShutdownReleasesResources,
                  rootRestart)
