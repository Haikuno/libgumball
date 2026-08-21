#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_logger.h>
#include <raylib.h>

static void GUM_raylibTraceLog_(int logLevel, const char* pText, va_list args) {
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), pText, args);

    switch (logLevel) {
        case LOG_ALL:
        case LOG_TRACE:
        case LOG_DEBUG:
            GUM_LOG_DEBUG("raylib: %s", buffer);
            break;
        case LOG_INFO:
            GUM_LOG_INFO("raylib: %s", buffer);
            break;
        case LOG_WARNING:
            GUM_LOG_WARN("raylib: %s", buffer);
            break;
        case LOG_ERROR:
        case LOG_FATAL:
            GUM_LOG_ERROR("raylib: %s", buffer);
            break;
    }
}

void GUM_Backend_setLogger(void) {
    SetTraceLogCallback(GUM_raylibTraceLog_);
}

void GUM_Backend_resetLogger(void) {
    SetTraceLogCallback(nullptr);
}
