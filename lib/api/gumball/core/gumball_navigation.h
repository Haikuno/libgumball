#ifndef GUM_NAVIGATION_H
#define GUM_NAVIGATION_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__navigation_8h.html

/*!  \file
 *   \brief   Keyboard/controller widget navigation
 *   \ingroup core
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gumball/devices/gumball_inputdevice.h>
#include <gumball/events/gumball_event_input.h>
#include <gumball/elements/gumball_widget.h>

GBL_DECLS_BEGIN

//! Moves focus in the given direction. With no focus, selects the default or first selectable Widget.
void GUM_Nav_move (GUM_InputDevice* pDevice, GUM_InputAction direction) GBL_NOEXCEPT;

//! Sets pDevice's focus to pWidget. nullptr clears focus.
void GUM_Nav_focus(GUM_InputDevice* pDevice, GUM_Widget* pWidget)       GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUM_NAVIGATION_H
