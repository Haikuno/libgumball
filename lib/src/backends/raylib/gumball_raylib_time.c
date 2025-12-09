#include <gumball/core/gumball_backend.h>
#include <raylib.h>

uint32_t GUM_Backend_timestamp(void) {
	return (uint32_t)(GetTime() * 1000);
}