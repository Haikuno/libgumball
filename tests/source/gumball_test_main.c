#include <gimbal/test/gimbal_test_scenario.h>
#include <gumball/gumball.h>
#include "core/gumball_navigation_test_suite.h"
#include "elements/gumball_container_test_suite.h"
#include "types/gumball_animator_test_suite.h"
#include "types/gumball_rectangle_test_suite.h"

#if defined(GUM_TEST_BACKEND_SDL3)
#include <SDL3/SDL.h>
#include <gumball/backends/gumball_sdl3.h>

static SDL_Surface* pSurface_ = nullptr;
static SDL_Renderer* pSdlRenderer_ = nullptr;
static GUM_Renderer* pRenderer_ = nullptr;

static bool backendInit_(void) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) return false;

    pSurface_ = SDL_CreateSurface(320, 240, SDL_PIXELFORMAT_RGBA32);
    pSdlRenderer_ = pSurface_ ? SDL_CreateSoftwareRenderer(pSurface_) : nullptr;
    pRenderer_ = pSdlRenderer_ ? GUM_SDL3_Renderer_create(pSdlRenderer_) : nullptr;
    return pRenderer_ != nullptr;
}

static void backendDeinit_(void) {
    GUM_Renderer_destroy(pRenderer_);
    SDL_DestroyRenderer(pSdlRenderer_);
    SDL_DestroySurface(pSurface_);
    SDL_Quit();
}
#elif defined(GUM_TEST_BACKEND_RAYLIB)
#include <raylib.h>

static bool backendInit_(void) {
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(320, 240, "libGumball tests");
    return IsWindowReady();
}

static void backendDeinit_(void) {
    CloseWindow();
}
#endif

static GblClass* pPersistentClasses_[8];
static size_t persistentClassCount_ = 0;

static void pinPersistentClass_(GblType type) {
    GblClass* pClass = GblClass_refDefault(type);
    if (pClass)
        pPersistentClasses_[persistentClassCount_++] = pClass;
}

static void preparePersistentMetadata_(void) {
    /* GblTestScenario temporarily replaces the global allocation context while
     * it runs. libGimbal's type/property/signal registries are process-global,
     * so initialize libGumball's persistent metadata before entering the
     * tracked test context. */
    (void)GUM_IResource_type();

    (void)GUM_Event_type();
    (void)GUM_Event_Input_type();
    (void)GUM_Event_Key_type();
    (void)GUM_Event_Gamepad_type();
    (void)GUM_Event_Mouse_type();

    (void)GUM_InputDevice_type();
    (void)GUM_Mouse_type();
    (void)GUM_Gamepad_type();
    (void)GUM_Keyboard_type();

    (void)GUM_Font_type();
    (void)GUM_Texture_type();
    (void)GUM_Color_type();
    (void)GUM_Rectangle_type();
    (void)GUM_Vector2_type();
    (void)GUM_Renderer_type();
    (void)GUM_Animator_type();

    (void)GUM_Root_type();
    (void)GUM_Widget_type();
    (void)GUM_Button_type();
    (void)GUM_Container_type();
    (void)GUM_ObjectViewer_type();

    (void)GUM_Manager_type();

    /* Keep the classes exercised by the current suites alive for the whole
     * tracked run. Their property/signal registration is global metadata and
     * must not resize global registries through the scenario allocator. */
    pinPersistentClass_(GUM_INPUTDEVICE_TYPE);
    pinPersistentClass_(GUM_MOUSE_TYPE);
    pinPersistentClass_(GUM_GAMEPAD_TYPE);
    pinPersistentClass_(GUM_KEYBOARD_TYPE);
    pinPersistentClass_(GUM_WIDGET_TYPE);
    pinPersistentClass_(GUM_BUTTON_TYPE);
    pinPersistentClass_(GUM_CONTAINER_TYPE);
    pinPersistentClass_(GUM_ROOT_TYPE);

    /* Exercise one representative hierarchy in the normal process context.
     * This initializes the process-global module/draw/runtime metadata that is
     * lazily grown on first real Widget use without charging it to a test suite. */
    GUM_Root* pRoot = GUM_Root_create();
    if (pRoot) {
        GUM_Container* pContainer = GUM_Container_create("w", 100.0f,
                                                         "h", 100.0f,
                                                         "padding", 10.0f,
                                                         "margin", 5.0f);
        if (pContainer) {
            (void)GUM_Widget_create("parent", pContainer);
            (void)GUM_Widget_create("parent", pContainer);
        }

        GUM_Root* pRequiredRoot = nullptr;
        GBL_REQUIRE_SCOPE(GUM_Root, &pRequiredRoot, "GUM_Root") {
        }

        GUM_unref(pRoot);
    }
}

static void releasePersistentMetadata_(void) {
    while (persistentClassCount_ != 0)
        GblClass_unrefDefault(pPersistentClasses_[--persistentClassCount_]);
}

int main(int argc, const char* pArgv[]) {
    if (!backendInit_()) return 1;

    preparePersistentMetadata_();

    GblTestScenario* pScenario = GblTestScenario_create("libGumballTests");

    GblContext_setLogFilter(GBL_CONTEXT(pScenario), GBL_LOG_LEVEL_INFO |
                                                    GBL_LOG_LEVEL_WARNING |
                                                    GBL_LOG_LEVEL_ERROR);

    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GUM_ANIMATOR_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GUM_RECTANGLE_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GUM_CONTAINER_TEST_SUITE_TYPE));
    GblTestScenario_enqueueSuite(pScenario,
                                 GblTestSuite_create(GUM_NAVIGATION_TEST_SUITE_TYPE));

    int result = GblTestScenario_exec(pScenario, argc, pArgv);

    GUM_IResource* pInvalidFont = GUM_Manager_load("invalid.ttf");
    if (pInvalidFont) {
        result = 1;
        GUM_Manager_unload(pInvalidFont);
        GUM_IResource_unref(pInvalidFont);
    }

    GUM_IResource* pInvalidTexture = GUM_Manager_load("invalid.png");
    if (pInvalidTexture) {
        result = 1;
        GUM_Manager_unload(pInvalidTexture);
        GUM_IResource_unref(pInvalidTexture);
    }

    GUM_IResource* pShutdownTexture = GUM_Manager_load("psyoplogo.png");
    if (!pShutdownTexture)
        result = 1;

    GUM_Font* pShutdownFont = GUM_FONT(GblBox_create(GUM_FONT_TYPE));
    if (!pShutdownFont) {
        result = 1;
    } else {
        GUM_Font_setDefault(pShutdownFont);
    }

    releasePersistentMetadata_();

    if (GUM_Font_default()) {
        GUM_Font_setDefault(nullptr);
        result = 1;
    }

    if (pShutdownTexture && GUM_IResource_data(pShutdownTexture))
        result = 1;

    if (pShutdownTexture)
        GUM_IResource_unref(pShutdownTexture);
    if (pShutdownFont)
        GUM_IResource_unref(GUM_IRESOURCE(pShutdownFont));

    GUM_Root* pRestartRoot = GUM_Root_create();
    GUM_IResource* pRestartTexture = pRestartRoot ? GUM_Manager_load("psyoplogo.png") : nullptr;
    if (!pRestartRoot || !pRestartTexture) {
        result = 1;
    } else {
        GUM_Manager_unload(pRestartTexture);
        if (GUM_IResource_data(pRestartTexture))
            result = 1;
        GUM_IResource_unref(pRestartTexture);
    }

    if (pRestartRoot)
        GUM_unref(pRestartRoot);

    backendDeinit_();
    return result;
}
