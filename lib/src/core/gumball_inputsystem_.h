#ifndef GUMBALL_INPUTSYSTEM_PRIVATE_H
#define GUMBALL_INPUTSYSTEM_PRIVATE_H

#include <gumball/core/gumball_inputsystem.h>
#include <gumball/types/gumball_renderer.h>

GBL_FORWARD_DECLARE_STRUCT(GUM_Widget);

uint64_t   GUM_InputSystem_generation_      (void) GBL_NOEXCEPT;
void       GUM_InputSystem_widgetDestroyed_ (GUM_Widget* pWidget) GBL_NOEXCEPT;
GBL_RESULT GUM_InputSystem_drawFocusRings_  (GUM_Renderer* pRenderer) GBL_NOEXCEPT;

#endif // GUMBALL_INPUTSYSTEM_PRIVATE_H
