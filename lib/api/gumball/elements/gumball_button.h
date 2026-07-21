#ifndef GUM_BUTTON_H
#define GUM_BUTTON_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__button_8h.html

/*!  \file
 *   \ref        GUM_Button "GUM_Button data structure and hierarchy graph"
 *   \ingroup    elements
 *
 *   GUM_Button is a basic button element that can be selected and pressed,
 *   firing a signal you can connect a callback to.
 *
 *   \author    2025, 2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include "gumball_widget.h"

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_BUTTON_TYPE             (GBL_TYPEID      (GUM_Button))           //!< Returns the GUM_Button Type UUID
#define GUM_BUTTON(self)            (GBL_CAST        (GUM_Button, self))     //!< Casts an instance of a compatible element to a GUM_Button
#define GUM_BUTTON_CLASS(klass)     (GBL_CLASS_CAST  (GUM_Button, klass))    //!< Casts a  class    of a compatible element to a GUM_ButtonClass
#define GUM_BUTTON_CLASSOF(self)    (GBL_CLASSOF     (GUM_Button, self))     //!< Casts an instance of a compatible element to a GUM_ButtonClass
//! @}

#define GBL_SELF_TYPE                GUM_Button

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Button);

/*!  \struct  GUM_ButtonClass
 *   \extends GUM_WidgetClass
 *   \brief   GUM_Button structure
 *
 *    GUM_ButtonClass derives from GUM_WidgetClass, adding nothing new.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Button, GUM_Widget)

/*!  \class   GUM_Button
 *   \extends GUM_Widget
 *   \brief   Basic button element
 *
*/
GBL_INSTANCE_DERIVE_EMPTY(GUM_Button, GUM_Widget)

GblType GUM_Button_type(void) GBL_NOEXCEPT;

//! Returns a new GUM_Button. Optionally takes in a list of Name/Value pairs for properties
#define GUM_Button_create(/* propertyName, propertyValue */ ...) GBL_NEW(GUM_Button __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_BUTTON_H
