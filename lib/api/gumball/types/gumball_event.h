#ifndef GUM_EVENT_H
#define GUM_EVENT_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__event_8h.html

/*! \file
 *  \ingroup types
 *
 *  GUM_Event is the base event type used in libGumball.
 *
 *  \author 2025 Agustín Bellagamba
 *  \copyright MIT License
*/

#include <gimbal/gimbal_meta.h>

/*! \name  Type System
 *  \brief Type UUID and Cast Operators
 *  @{
*/
#define GUM_EVENT_TYPE				(GBL_TYPEID(GUM_Event)) 			//!< Returns the GUM_Event Type UUID
#define GUM_EVENT(self)				(GBL_CAST(GUM_Event, self))			//!< Casts an instance of a compatible element to a GUM_Event
#define GUM_EVENT_CLASS(klass)		(GBL_CLASS_CAST(GUM_Event, klass))	//!< Casts an class of a compatible element to a GUM_Event
#define GUM_EVENT_CLASSOF(self)		(GBL_CLASSOF(GUM_Event, self))		//!< Casts an instance of a compatible element to a GUM_Event
//! @}

#define GBL_SELF_TYPE GUM_Event

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Event);

/*! \struct  GUM_EventClass
 *  \extends GblEventClass
 *  \brief   GUM_Event structure
 *
 *  GUM_EventClass derives from GblEvent,
 * 	adding nothing new.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Event, GblEvent)

/*!
 *	\class   GUM_Event
 *	\extends GblEvent
 *	\brief   Basic event type
 *
*/
GBL_INSTANCE_DERIVE_EMPTY(GUM_Event, GblEvent)

//! \cond
GblType GUM_Event_type(void);
//! \endcond

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_EVENT_H
