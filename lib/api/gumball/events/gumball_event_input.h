#ifndef GUM_EVENT_INPUT_H
#define GUM_EVENT_INPUT_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__event__input_8h.html

/*! \file
 *  \ingroup events
 *
 *  Base event type for input
 *
 *  \author 	2025 Agustín Bellagamba
 *  \copyright 	MIT License
*/

#include "gumball_event.h"

/*! \name  Type System
 *  \brief Type UUID and Cast Operators
 *  @{
*/
#define GUM_EVENT_INPUT_TYPE			(GBL_TYPEID		(GUM_Event_Input)) 			//!< Returns the GUM_Event_Input Type UUID
#define GUM_EVENT_INPUT(self)			(GBL_CAST		(GUM_Event_Input, self))	//!< Casts an instance of a compatible element to a GUM_Event_Input
#define GUM_EVENT_INPUT_CLASS(klass)	(GBL_CLASS_CAST	(GUM_Event_Input, klass))	//!< Casts a  class    of a compatible element to a GUM_Event_InputClass
#define GUM_EVENT_INPUT_CLASSOF(self)	(GBL_CLASSOF	(GUM_Event_Input, self))	//!< Casts an instance of a compatible element to a GUM_Event_InputClass
//! @}

#define GBL_SELF_TYPE GUM_Event_Input

GBL_DECLS_BEGIN

typedef enum : uint8_t {
    GUM_INPUT_PRESS,
    GUM_INPUT_RELEASE
} GUM_INPUT_STATE;

typedef enum : uint8_t {
	GUM_INPUT_PRIMARY,
	GUM_INPUT_SECONDARY,
	GUM_INPUT_TERTIARY,
	GUM_INPUT_LEFT,
	GUM_INPUT_UP,
	GUM_INPUT_RIGHT,
	GUM_INPUT_DOWN
} GUM_INPUT_ID;

GBL_FORWARD_DECLARE_STRUCT(GUM_Event_Input);

/*! \struct  GUM_Event_InputClass
 *	\extends GUM_EventClass
 *  \brief   GUM_Event_Input structure
 *
 *	GUM_Event_InputClass derives from GUM_EventClass, adding nothing new.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Event_Input, GUM_Event);

/*!
 *	\class   GUM_Event_Input
 *	\extends GUM_Event
 *	\brief   Input event
 *
 * 	GUM_Event_Input represents any input event.
 *
*/
GBL_INSTANCE_DERIVE(GUM_Event_Input, GUM_Event)
	GUM_INPUT_STATE state;		//!< State of the input event
	GUM_INPUT_ID 	id;			//!< ID of the input event
GBL_INSTANCE_END

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif