#ifndef GUM_EVENT_KEY_H
#define GUM_EVENT_KEY_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__event__key_8h.html

/*!  \file
 *   \ingroup events
 *
 *   Event type for key events
 *
 *   \author     2025 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include "gumball_event_input.h"

/*!  \name  Type System
 *   \brief Type UUID and Cast Operators
 *   @{
*/
#define GUM_EVENT_KEY_TYPE            (GBL_TYPEID     (GUM_Event_Key))         //!< Returns the GUM_Event_Key Type UUID
#define GUM_EVENT_KEY(self)           (GBL_CAST       (GUM_Event_Key, self))   //!< Casts an instance of a compatible event to a GUM_Event_Key
#define GUM_EVENT_KEY_CLASS(klass)    (GBL_CLASS_CAST (GUM_Event_Key, klass))  //!< Casts a  class    of a compatible event to a GUM_Event_KeyClass
#define GUM_EVENT_KEY_CLASSOF(self)   (GBL_CLASSOF    (GUM_Event_Key, self))   //!< Casts an instance of a compatible event to a GUM_Event_KeyClass
//! @}

#define GBL_SELF_TYPE GUM_Event_Key

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Event_Key);

/*!  \struct  GUM_Event_KeyClass
 *   \extends GUM_EventClass
 *   \brief   GUM_Event_Key structure
 *
 *   GUM_Event_KeyClass derives from GUM_EventClass, adding nothing new.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Event_Key, GUM_Event_Input);

/*!  \class   GUM_Event_Key
 *   \extends GUM_Event
 *   \brief   Key event
 *
 *   GUM_Event_Key represents any key event, such as pressing or releasing a key.
 *
*/
GBL_INSTANCE_DERIVE(GUM_Event_Key, GUM_Event_Input)
    // TODO: add flag modifier here (such as holding shift)
GBL_INSTANCE_END

#endif
