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

static void registerPersistentTypes_(void) {
    /* GblTestScenario temporarily replaces the global allocation context while
     * it runs. libGimbal's type registry is process-global, so keep all
     * persistent libGumball type metadata in the process allocation context
     * instead of first registering types inside the tracked test context. */
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
}

int main(int argc, const char* pArgv[]) {
    if (!backendInit_()) return 1;

    registerPersistentTypes_();

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

    const int result = GblTestScenario_exec(pScenario, argc, pArgv);
    backendDeinit_();
    return result;
}
