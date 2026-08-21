#include <SDL3/SDL.h>
#include <gumball/core/gumball_backend.h>

static SDL_LogOutputFunction pPreviousLogger_ = nullptr;
static void*                 pPreviousLoggerData_ = nullptr;
static bool                  loggerInstalled_ = false;

static void SDLCALL GUM_SDL3_log_(void* pUserdata, int category, SDL_LogPriority priority, const char* pMessage) {
    GBL_UNUSED(pUserdata, category);

    switch (priority) {
        case SDL_LOG_PRIORITY_TRACE:
        case SDL_LOG_PRIORITY_VERBOSE:
        case SDL_LOG_PRIORITY_DEBUG:
            GBL_LOG_DEBUG("SDL3 ", pMessage);
            break;
        case SDL_LOG_PRIORITY_INFO:
            GBL_LOG_INFO("SDL3 ", pMessage);
            break;
        case SDL_LOG_PRIORITY_WARN:
            GBL_LOG_WARN("SDL3 ", pMessage);
            break;
        case SDL_LOG_PRIORITY_ERROR:
        case SDL_LOG_PRIORITY_CRITICAL:
            GBL_LOG_ERROR("SDL3 ", pMessage);
            break;
        default:
            break;
    }
}

void GUM_Backend_setLogger(void) {
    if (loggerInstalled_) return;

    SDL_GetLogOutputFunction(&pPreviousLogger_, &pPreviousLoggerData_);
    SDL_SetLogOutputFunction(GUM_SDL3_log_, nullptr);
    loggerInstalled_ = true;
}

void GUM_Backend_resetLogger(void) {
    if (!loggerInstalled_) return;

    SDL_SetLogOutputFunction(pPreviousLogger_, pPreviousLoggerData_);
    pPreviousLogger_     = nullptr;
    pPreviousLoggerData_ = nullptr;
    loggerInstalled_     = false;
}
