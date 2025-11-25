#ifndef GUM_CONTAINER_H
#define GUM_CONTAINER_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__container_8h.html

/*! \file
 *  \ref     GUM_Container "GUM_Container data structure and hierarchy graph"
 *  \ingroup elements
 *
 * 	GUM_Container is a container element that can hold other widgets.
 *  It can be used to group widgets together, and to optionally automatically resize / align them as needed.
 *
 *  \todo
 *  	- Make variables private
 *      - Make orientation an enum
 *      - Add scrolling
 *
 *  \author 2025 Agustín Bellagamba
 *  \copyright MIT License
*/

#include "gumball_widget.h"

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_CONTAINER_TYPE				(GBL_TYPEID     (GUM_Container))            //!< Returns the GUM_Container Type UUID
#define GUM_CONTAINER(self)				(GBL_CAST       (GUM_Container, self))      //!< Casts an instance of a compatible element to a GUM_Container
#define GUM_CONTAINER_CLASS(klass)		(GBL_CLASS_CAST (GUM_Container, klass))     //!< Casts an class of a compatible element to a GUM_ContainerClass
#define GUM_CONTAINER_CLASSOF(self)	    (GBL_CLASSOF    (GUM_Container, self))      //!< Casts an instance of a compatible element to a GUM_Container
//! @}

#define GBL_SELF_TYPE                    GUM_Container

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Container);

/*!
    \struct  GUM_ContainerClass
    \extends GUM_WidgetClass
    \brief   GUM_Container structure

    GUM_ContainerClass derives from GUM_WidgetClass,
    adding a virtual function to update the content of the container.
*/
GBL_CLASS_DERIVE(GUM_Container, GUM_Widget)
    GBL_RESULT (*pFnUpdateContent)(GBL_SELF); //!< Updates the content of the container, resizing and realigning child widgets as needed.
GBL_CLASS_END

/*!
 *	\class   GUM_Container
 *	\extends GUM_Widget
 *	\brief   Container element
 *
*/

/*!
	\name  Properties
	\brief Properties you can set/get at or after creation.
    \note  You can also set/get properties from parent classes (see \ref GUM_Container).
	@{
*/
GBL_INSTANCE_DERIVE(GUM_Container, GUM_Widget)
    char orientation;       //!< 'h' for horizontal, 'v' for vertical layout of child widgets.                          Default value is 'v'    \warning This is planned to be replaced by an enum.
    bool resizeWidgets;     //!< If child widgets should be resized to take an equal amount of space.                   Default value is true
    bool alignWidgets;      //!< If child widgets should be aligned.                                                    Default value is true
    float padding;          //!< The space between the container's border and its child widgets.                        Default value is 5
    float margin;           //!< The space between child widgets.                                                       Default value is 5
GBL_INSTANCE_END
//! @}

GBL_PROPERTIES(GUM_Container,
    (orientation,       GBL_GENERIC, (READ, WRITE), GBL_CHAR_TYPE),
    (resizeWidgets,     GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE),
    (alignWidgets,      GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE),
    (padding,           GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE),
    (margin,            GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE)
)

GblType GUM_Container_type(void);

//! Returns a new GUM_Container. Optionally takes in a list of Name/Value pairs for properties
#define GUM_Container_create(/* propertyName, propertyValue */ ...) GBL_NEW(GUM_Container __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_CONTAINER_H
