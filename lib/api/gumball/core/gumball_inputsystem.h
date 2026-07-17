#ifndef GUM_INPUTSYSTEM_H
#define GUM_INPUTSYSTEM_H

// View this file's documentation online: TODO: add link

/*!  \file
 *   \brief     Input system
 *   \ingroup   core
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gumball/events/gumball_event_input.h>

typedef struct GUM_InputBinding {
    GblType  deviceType;
    GblFlags button;
} GUM_InputBinding;

// Initializes the input system
void       GUM_InputSystem_init     (void);

// Deinitializes the input system
void       GUM_InputSystem_deinit   (void);

// Updates the input system
void       GUM_InputSystem_update   (void);

// Given a deviceType, binds an action to the passed button.
GBL_RESULT GUM_InputSystem_bind     (GblType deviceType, GUM_InputAction action, GblFlags button);
// Given a deviceType, unbinds an action previously associated with the passed button, if it was bound in the first place.
GBL_RESULT GUM_InputSystem_unbind   (GblType deviceType, GUM_InputAction action, GblFlags button);

#endif
