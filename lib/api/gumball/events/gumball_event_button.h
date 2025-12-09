#ifndef GUM_EVENT_BUTTON_H
#define GUM_EVENT_BUTTON_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__event__button_8h.html

/*! \file
 *  \ingroup events
 *
 *  Event type for gamepad button events
 *
 *  \author 	2025 Agustín Bellagamba
 *  \copyright 	MIT License
*/

#include "gumball_event_input.h"

/*! \name  Type System
 *  \brief Type UUID and Cast Operators
 *  @{
*/
#define GUM_EVENT_BUTTON_TYPE				(GBL_TYPEID		(GUM_Event_Button)) 		//!< Returns the GUM_Event_Button Type UUID
#define GUM_EVENT_BUTTON(self)				(GBL_CAST		(GUM_Event_Button, self))	//!< Casts an instance of a compatible event to a GUM_Event_Button
#define GUM_EVENT_BUTTON_CLASS(klass)		(GBL_CLASS_CAST	(GUM_Event_Button, klass))	//!< Casts a  class    of a compatible event to a GUM_Event_ButtonClass
#define GUM_EVENT_BUTTON_CLASSOF(self)		(GBL_CLASSOF	(GUM_Event_Button, self))	//!< Casts an instance of a compatible event to a GUM_Event_ButtonClass
//! @}

#define GBL_SELF_TYPE GUM_Event_Button

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Event_Button);
z
/*! \struct  GUM_Event_ButtonClass
 *	\extends GUM_EventClass
 *  \brief   GUM_Event_Button class
 *
 *	GUM_Event_ButtonClass derives from GUM_EventClass, adding nothing new.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Event_Button, GUM_Event_Input);

/*!
 *	\class   GUM_Event_Button
 *	\extends GUM_Event
 *	\brief   Gamepad button event
 *
 * 	GUM_Event_Button represents any gamepad button event, such as pressing or releasing a controller button.
 *
*/
GBL_INSTANCE_DERIVE(GUM_Event_Button, GUM_Event_Input)
	uint8_t controllerId; //!< ID of the controller that generated the event
GBL_INSTANCE_END

#endif