#ifndef GUM_CONTAINER_H
#define GUM_CONTAINER_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__container_8h.html

/*!  \file
 *   \ref     GUM_Container "GUM_Container data structure and hierarchy graph"
 *   \ingroup elements
 *
 *   GUM_Container is a container element that can hold other widgets.
 *   It can group, resize, align, and scroll widgets.
 *
 *   \author     2025, 2026 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include "gimbal/meta/classes/gimbal_primitives.h"
#include "gumball_widget.h"
#include <stdint.h>

/*!  \name  Type System
 *   \brief Type UUID and cast operators
 *   @{
*/
#define GUM_CONTAINER_TYPE                (GBL_TYPEID     (GUM_Container))            //!< Returns the GUM_Container Type UUID
#define GUM_CONTAINER(self)               (GBL_CAST       (GUM_Container, self))      //!< Casts an instance of a compatible element to a GUM_Container
#define GUM_CONTAINER_CLASS(klass)        (GBL_CLASS_CAST (GUM_Container, klass))     //!< Casts a class of a compatible element to a GUM_ContainerClass
#define GUM_CONTAINER_CLASSOF(self)       (GBL_CLASSOF    (GUM_Container, self))      //!< Gets the GUM_ContainerClass for a compatible instance
//! @}

#define GBL_SELF_TYPE GUM_Container

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Container);

/*!  \struct  GUM_ContainerClass
 *   \extends GUM_WidgetClass
 *   \brief   GUM_Container class structure
*/
GBL_CLASS_DERIVE(GUM_Container, GUM_Widget)
    GBL_RESULT (*pFnUpdateContent)(GBL_SELF); //!< Updates child layout and clipping.
GBL_CLASS_END

GBL_ENUM(GUM_Direction,
    (GUM_DIRECTION_NULL,       "GUM_orient_n", 0),
    (GUM_DIRECTION_VERTICAL,   "GUM_orient_v", 1),
    (GUM_DIRECTION_HORIZONTAL, "GUM_orient_h", 2)
)

/*!  \class   GUM_Container
 *   \extends GUM_Widget
 *   \brief   Container element
*/
GBL_INSTANCE_DERIVE_EMPTY(GUM_Container, GUM_Widget)

/*  \name  Properties
 *  \brief Properties you can set/get at or after creation.
 *  \note  You can also set/get properties from parent classes.
 *  @{
*/
GBL_PROPERTIES(GUM_Container,
    (padding,           GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE           ),
    (margin,            GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE           ),
    (minChildSize,      GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE           ),
    (direction,         GBL_GENERIC, (READ, WRITE), GBL_TYPEID(GUM_Direction)),
    (resizeWidgets,     GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE            ),
    (alignWidgets,      GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE            ),
    (scrollable,        GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE            )
)
//! @}

GBL_EXPORT GblType       GUM_Container_type          (void) GBL_NOEXCEPT;
GBL_EXPORT float         GUM_Container_padding       (const GUM_Container* pSelf) GBL_NOEXCEPT; //!< Border-to-content padding. Default: 5.
GBL_EXPORT float         GUM_Container_margin        (const GUM_Container* pSelf) GBL_NOEXCEPT; //!< Spacing between children. Default: 2.
GBL_EXPORT float         GUM_Container_minChildSize  (const GUM_Container* pSelf) GBL_NOEXCEPT; //!< Minimum main-axis child size. Default: 0.15.
GBL_EXPORT GUM_Direction GUM_Container_direction     (const GUM_Container* pSelf) GBL_NOEXCEPT; //!< Current layout direction.
GBL_EXPORT GblBool       GUM_Container_resizeWidgets (const GUM_Container* pSelf) GBL_NOEXCEPT; //!< Whether children are resized automatically.
GBL_EXPORT GblBool       GUM_Container_alignWidgets  (const GUM_Container* pSelf) GBL_NOEXCEPT; //!< Whether children are positioned automatically.
GBL_EXPORT GblBool       GUM_Container_scrollable    (const GUM_Container* pSelf) GBL_NOEXCEPT; //!< Whether overflowing content may scroll.

//! Returns a new GUM_Container. Optionally takes a list of Name/Value pairs for properties.
#define GUM_Container_create(/* propertyName, propertyValue */ ...) GBL_NEW(GUM_Container __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_CONTAINER_H
