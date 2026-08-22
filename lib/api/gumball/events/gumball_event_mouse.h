#ifndef GUM_EVENT_MOUSE_H
#define GUM_EVENT_MOUSE_H

// View this file's documentation online: TODO: add link

/*!  \file
 *   \ref     GUM_Event_Mouse "GUM_Event_Mouse data structure and hierarchy graph"
 *   \ingroup events
 *
 *   Mouse button event. In addition to the common input state/action fields,
 *   it snapshots pointer position, movement delta, and wheel delta from the
 *   emitting mouse device.
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gumball/events/gumball_event_pointer.h>

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
GBL_FORWARD_DECLARE_STRUCT(GUM_Mouse);

GBL_CLASS_DERIVE_EMPTY(GUM_Event_Mouse, GUM_Event_Pointer)

GBL_INSTANCE_DERIVE(GUM_Event_Mouse, GUM_Event_Pointer)
    GUM_Vector2 delta; //!< Mouse movement delta when the event was created.
    GUM_Vector2 wheel; //!< Wheel delta when the event was created.
GBL_INSTANCE_END

GblType GUM_Event_Mouse_type(void) GBL_NOEXCEPT;

//! Returns a new zero-initialized GUM_Event_Mouse.
#define GUM_Event_Mouse_create() GUM_EVENT_MOUSE(GblEvent_create(GUM_EVENT_MOUSE_TYPE))
//! Creates a mouse event and snapshots position/delta/wheel/device from pMouse.
GUM_Event_Mouse* GUM_Event_Mouse_createFrom(GUM_Mouse* pMouse) GBL_NOEXCEPT;

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_EVENT_MOUSE_H
