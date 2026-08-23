#include <gumball/core/gumball_backend.h>

void GUM_Backend_setLogger(void) {
    // raylib cannot restore an existing trace callback, so leave logging to the host.
}

void GUM_Backend_resetLogger(void) {}
