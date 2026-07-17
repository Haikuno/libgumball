#ifndef GUM_EVENT_INPUT_H
#define GUM_EVENT_INPUT_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__event__input_8h.html

/*!  \file
 *   \ingroup events
 *
 *   Base event type for input
 *
 *   \author     2025, 2026 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include "gumball_event.h"
#include <gumball/devices/gumball_inputdevice.h>
#include <gimbal/meta/classes/gimbal_enum.h>
#include <gimbal/containers/gimbal_array_list.h>

/*!  \name  Type System
 *   \brief Type UUID and Cast Operators
 *   @{
*/
#define GUM_EVENT_INPUT_TYPE            (GBL_TYPEID     (GUM_Event_Input))          //!< Returns the GUM_Event_Input Type UUID
#define GUM_EVENT_INPUT(self)           (GBL_CAST       (GUM_Event_Input, self))    //!< Casts an instance of a compatible element to a GUM_Event_Input
#define GUM_EVENT_INPUT_CLASS(klass)    (GBL_CLASS_CAST (GUM_Event_Input, klass))   //!< Casts a  class    of a compatible element to a GUM_Event_InputClass
#define GUM_EVENT_INPUT_CLASSOF(self)   (GBL_CLASSOF    (GUM_Event_Input, self))    //!< Casts an instance of a compatible element to a GUM_Event_InputClass
//! @}

#define GBL_SELF_TYPE GUM_Event_Input

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GUM_Event_Input);

/*!  \struct  GUM_Event_InputClass
 *   \extends GUM_EventClass
 *   \brief   GUM_Event_Input structure
 *
 *   \todo
 *      - Add more actions.
 *
 *   GUM_Event_InputClass derives from GUM_EventClass, adding nothing new.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Event_Input, GUM_Event);

GBL_ENUM(GUM_InputState,
    (GUM_INPUTSTATE_NULL,    "GUM_IS_null",    0),
    (GUM_INPUTSTATE_PRESS,   "GUM_IS_press",   1),
    (GUM_INPUTSTATE_RELEASE, "GUM_IS_release", 2)
)

GBL_ENUM(GUM_InputAction,
    (GUM_INPUTACTION_NULL,    "GUM_IA_null",     0), // buttonaction not initialized
    (GUM_INPUTACTION_CONFIRM, "GUM_IA_confirm",  1),
    (GUM_INPUTACTION_CANCEL,  "GUM_IA_cancel",   2),
    (GUM_INPUTACTION_COUNT,   "GUM_IA_count",    3),
    (GUM_INPUTACTION_UNBOUND, "GUM_IA_unbound",  4)  // button not bound to an action
)

/*!  \class   GUM_Event_Input
 *   \extends GUM_Event
 *   \brief   Input event
 *
 *   GUM_Event_Input represents any input event.
 *
*/
GBL_INSTANCE_DERIVE(GUM_Event_Input, GUM_Event)
    GUM_InputDevice* pInputDevice; //!< Pointer to the input device that emitted the event
    GblFlags button;               //!< Bitmask of the button involved
    GUM_InputState state;          //!< Input state (pressed / released)
    GUM_InputAction action;        //!< Semantic action
GBL_INSTANCE_END

GblType GUM_Event_Input_type(void);

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif
