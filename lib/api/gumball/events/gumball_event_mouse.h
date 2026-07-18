#ifndef GUM_EVENT_MOUSE_H
#define GUM_EVENT_MOUSE_H

// View this file's documentation online: TODO: add link

/*!  \file
 *   \ref     GUM_Event_Mouse "GUM_Event_Mouse data structure and hierarchy graph"
 *   \ingroup events
 *
 *   Brief description
 *
 *   Extended
 *   Description.
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gumball/events/gumball_event_input.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_EVENT_MOUSE_TYPE            (GBL_TYPEID     (GUM_Event_Mouse))
#define GUM_EVENT_MOUSE(self)           (GBL_CAST       (GUM_Event_Mouse, self))
#define GUM_EVENT_MOUSE_CLASS(klass)    (GBL_CLASS_CAST (GUM_Event_Mouse, klass))
#define GUM_EVENT_MOUSE_CLASSOF(self)   (GBL_CLASSOF    (GUM_Event_Mouse, self))
//! @}

#define GBL_SELF_TYPE GUM_Event_Mouse

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Event_Mouse);

/*!
 *    \struct  GUM_Event_MouseClass
 *    \extends GUM_Event_InputClass
 *    \brief   GUM_Event_Mouse structure
 *
 *    GUM_Event_MouseClass derives from GUM_Event_InputClass,
 *    TODO: description
*/
//! \cond
GBL_CLASS_DERIVE(GUM_Event_Mouse, GUM_Event_Input)
    //
GBL_CLASS_END
//! \endcond

/*!
 *    \class   GUM_Event_Mouse
 *    \extends GUM_Event_Input
 *    \brief   TODO: brief description
*/
GBL_INSTANCE_DERIVE(GUM_Event_Mouse, GUM_Event_Input)
    //
GBL_INSTANCE_END

GblType GUM_Event_Mouse_type(void) GBL_NOEXCEPT;

//! Returns a new GUM_Event_Mouse.
#define GUM_Event_Mouse_create() GUM_EVENT_MOUSE(GblEvent_create(GUM_EVENT_MOUSE_TYPE))

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_EVENT_MOUSE_H