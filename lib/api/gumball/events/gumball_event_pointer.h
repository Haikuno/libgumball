#ifndef GUM_EVENT_POINTER_H
#define GUM_EVENT_POINTER_H

// View this file's documentation online: TODO: add link

/*!  \file
 *   \ref     GUM_Event_Pointer "GUM_Event_Pointer data structure and hierarchy graph"
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
#define GUM_EVENT_POINTER_TYPE            (GBL_TYPEID     (GUM_Event_Pointer))
#define GUM_EVENT_POINTER(self)           (GBL_CAST       (GUM_Event_Pointer, self))
#define GUM_EVENT_POINTER_CLASS(klass)    (GBL_CLASS_CAST (GUM_Event_Pointer, klass))
#define GUM_EVENT_POINTER_CLASSOF(self)   (GBL_CLASSOF    (GUM_Event_Pointer, self))
//! @}

#define GBL_SELF_TYPE GUM_Event_Pointer

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Event_Pointer);

/*!
 *    \struct  GUM_Event_PointerClass
 *    \extends GUM_Event_InputClass
 *    \brief   GUM_Event_Pointer structure
 *
 *    GUM_Event_PointerClass derives from GUM_Event_InputClass,
 *    TODO: description
*/
//! \cond
GBL_CLASS_DERIVE(GUM_Event_Pointer, GUM_Event_Input)
    //
GBL_CLASS_END
//! \endcond

/*!
 *    \class   GUM_Event_Pointer
 *    \extends GUM_Event_Input
 *    \brief   TODO: brief description
*/

GBL_INSTANCE_DERIVE(GUM_Event_Pointer, GUM_Event_Input)
    //
GBL_INSTANCE_END

//! \cond
GblType GUM_Event_Pointer_type(void) GBL_NOEXCEPT;
//! \endcond

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_EVENT_POINTER_H