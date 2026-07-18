#ifndef GUM_NAVIGATION_H
#define GUM_NAVIGATION_H

// View this file's documentation online: TODO: add link

/*!  \file
 *   \brief     Keyboard/controller widget navigation
 *   \ingroup   core
 *
 *   gumball_navigation implements directional widget-to-widget focus movement
 *   for any GUM_InputDevice
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gumball/devices/gumball_inputdevice.h>
#include <gumball/events/gumball_event_input.h>
#include <gumball/elements/gumball_widget.h>

GBL_DECLS_BEGIN

/*!  Moves \p pDevice's navigation focus one step in the given direction
 *   (GUM_INPUTACTION_MOVE_UP/DOWN/LEFT/RIGHT).
 *
 *   If the device doesn't currently have a focused widget, this instead
 *   acquires one (preferring a widget with \c isSelectedByDefault set,
 *   falling back to the first selectable widget found) and does not move.
 *
 *   Emits \c onFocusLost on the previously-focused widget and \c onFocusGained
 *   on the newly-focused widget, both passing \p pDevice as the argument.
 *
 *   No-op if \p direction isn't one of the four movement actions.
*/
void GUM_Nav_move(GUM_InputDevice* pDevice, GUM_InputAction direction) GBL_NOEXCEPT;

/*!  Explicitly sets \p pDevice's navigation focus to \p pWidget (or clears it,
 *   if \p pWidget is \c nullptr), emitting \c onFocusLost / \c onFocusGained
 *   as appropriate.
*/
void GUM_Nav_focus(GUM_InputDevice* pDevice, GUM_Widget* pWidget) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUM_NAVIGATION_H
