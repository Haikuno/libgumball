#ifndef GUM_EVENT_H
#define GUM_EVENT_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__event_8h.html

/*!  \file
 *   \ingroup events
 *
 *   GUM_Event is the base event type used in libGumball.
 *
 *   \author     2025 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include <gimbal/gimbal_meta.h>

/*!  \name  Type System
 *   \brief Type UUID and Cast Operators
 *   @{
*/
#define GUM_EVENT_TYPE            (GBL_TYPEID      (GUM_Event))         //!< Returns the GUM_Event Type UUID
#define GUM_EVENT(self)           (GBL_CAST        (GUM_Event, self))   //!< Casts an instance of a compatible event to a GUM_Event
#define GUM_EVENT_CLASS(klass)    (GBL_CLASS_CAST  (GUM_Event, klass))  //!< Casts a  class    of a compatible event to a GUM_EventClass
#define GUM_EVENT_CLASSOF(self)   (GBL_CLASSOF     (GUM_Event, self))   //!< Casts an instance of a compatible event to a GUM_EventClass
//! @}

#define GBL_SELF_TYPE GUM_Event

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Event);

/*!  \struct  GUM_EventClass
 *   \extends GblEventClass
 *   \brief   GUM_Event structure
 *
 *   GUM_EventClass derives from GblEvent, adding nothing new.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Event, GblEvent)

/*!  \class   GUM_Event
 *   \extends GblEvent
 *   \brief   Basic event type
 *
 *   GUM_Event is the base event type used in libGumball.
 *   It inhertis from GblEvent, and only adds a timestamp.
 *
*/
GBL_INSTANCE_DERIVE(GUM_Event, GblEvent)
    uint32_t timestamp; //!< Timestamp of the event in ms since program start, wraps after ~49 days
GBL_INSTANCE_END

//! \cond
GblType GUM_Event_type(void);
//! \endcond

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_EVENT_H
