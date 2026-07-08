#ifndef GUM_EVENT_GAMEPAD_H
#define GUM_EVENT_GAMEPAD_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__event__gamepad_8h.html

/*!  \file
 *   \ingroup events
 *
 *   Event type for gamepad events
 *
 *   \author     2025, 2026 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include <gumball/events/gumball_event_input.h>

/*!  \name  Type System
 *   \brief Type UUID and Cast Operators
 *   @{
*/
#define GUM_EVENT_GAMEPAD_TYPE          (GBL_TYPEID     (GUM_Event_Gamepad))           //!< Returns the GUM_Event_Gamepad Type UUID
#define GUM_EVENT_GAMEPAD(self)         (GBL_CAST       (GUM_Event_Gamepad, self))     //!< Casts an instance of a compatible event to a GUM_Event_Gamepad
#define GUM_EVENT_GAMEPAD_CLASS(klass)  (GBL_CLASS_CAST (GUM_Event_Gamepad, klass))    //!< Casts a  class    of a compatible event to a GUM_Event_GamepadClass
#define GUM_EVENT_GAMEPAD_CLASSOF(self) (GBL_CLASSOF    (GUM_Event_Gamepad, self))     //!< Casts an instance of a compatible event to a GUM_Event_GamepadClass
//! @}

#define GBL_SELF_TYPE GUM_Event_Gamepad

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Event_Gamepad);
/*!  \struct  GUM_Event_GamepadClass
 *   \extends GUM_EventClass
 *   \brief   GUM_Event_Gamepad class
 *
 *   GUM_Event_GamepadClass derives from GUM_EventClass, adding nothing new.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Event_Gamepad, GUM_Event_Input);

/*!  \class   GUM_Event_Gamepad
 *   \extends GUM_Event
 *   \brief   Gamepad event
 *
 *   GUM_Event_Gamepad represents any gamepad event, such as pressing or releasing a controller button.
*/
GBL_INSTANCE_DERIVE(GUM_Event_Gamepad, GUM_Event_Input)
    //
GBL_INSTANCE_END

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif
