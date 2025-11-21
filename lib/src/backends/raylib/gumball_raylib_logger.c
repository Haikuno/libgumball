#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_logger.h>
#include <raylib.h>

static void GUM_raylibTraceLog_(int logLevel, const char *text, va_list args) {
	char buf[1024];
    vsnprintf(buf, sizeof(buf), text, args);
	switch (logLevel) {
		case LOG_ALL:
		case LOG_TRACE:
		case LOG_DEBUG:
			GBL_LOG_DEBUG("raylib ", buf);
			break;
		case LOG_INFO:
			GBL_LOG_INFO("raylib ", buf);
			break;
		case LOG_WARNING:
			GBL_LOG_WARN("raylib ", buf);
			break;
		case LOG_ERROR:
		case LOG_FATAL:
			GBL_LOG_ERROR("raylib ", buf);
			break;
	}
}

void GUM_Backend_setLogger(void) {
	SetTraceLogCallback(GUM_raylibTraceLog_);
}

void GUM_Backend_resetLogger(void) {
	SetTraceLogCallback(nullptr);
}