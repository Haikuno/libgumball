#ifndef GUM_CONTROLLER_H
#define GUM_CONTROLLER_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__controller_8h.html

/*! \file
 *  \ref   	 GUM_Controller "GUM_Controller data structure and hierarchy graph"
 *  \ingroup elements
 *
 * 	GUM_Controller is responsible for handling input from gamepads and/or keyboards,
 *  and drawing a border around the currently selected button.
 *  Each controller is associated with a specific player id, allowing you to have multiple
 *  controllers at once.
 *
 * 	\todo
 * 	   - Handle multiple controllers on the same button
 * 	   - Make pSelectedButton a property
 * 	   - Make variables private
 *     - Deprecate GUM_Controller_setSelectedButton once GUM_Property is implemented
 *
 * 	\author 2025 Agustín Bellagamba
 *	\copyright MIT License
*/

#include "gumball_button.h"

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GUM_CONTROLLER_TYPE				(GBL_TYPEID		(GUM_Controller))
#define GUM_CONTROLLER(self)			(GBL_CAST		(GUM_Controller, self))
#define GUM_CONTROLLER_CLASS(klass)		(GBL_CLASS_CAST	(GUM_Controller, klass))
#define GUM_CONTROLLER_CLASSOF(self)	(GBL_CLASSOF	(GUM_Controller, self))
//! @}

#define GBL_SELF_TYPE 					 GUM_Controller

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Controller);

typedef enum {
    GUM_CONTROLLER_BUTTON_PRESS,
    GUM_CONTROLLER_BUTTON_RELEASE
} GUM_CONTROLLER_BUTTON_STATE;

typedef enum {
	GUM_CONTROLLER_UP,
	GUM_CONTROLLER_RIGHT,
	GUM_CONTROLLER_DOWN,
	GUM_CONTROLLER_LEFT,
	GUM_CONTROLLER_PRIMARY,
	GUM_CONTROLLER_SECONDARY,
	GUM_CONTROLLER_TERTIARY
} GUM_CONTROLLER_BUTTON_ID;

/*!
 *	\struct  GUM_ControllerClass
 *	\extends GUM_WidgetClass
 *	\brief 	 GUM_Controller structure
 *
 *	GUM_ControllerClass derives from GUM_WidgetClass,
 *	adding nothing new.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Controller, GUM_Widget)

/*!
 *	\struct  GUM_Controller
 *	\extends GUM_Widget
 *	\brief 	 Controller element responsible for handling input used for UI interaction
*/

/*!
 *	\name  Properties
 *	\brief Properties you can set/get at or after creation.
 *	\note  You can also set/get properties from parent classes (see \ref GUM_Controller).
 *	@{
*/
//! \cond
GBL_INSTANCE_DERIVE(GUM_Controller, GUM_Widget)
//! \endcond
	GUM_Button *pSelectedButton; // The currently selected button
	uint8_t		controllerId;	 //!< Which id is this controller associated with. Default value is 0
	bool		isKeyboard; 	 //!< If the controller is a keyboard. Default value is false
GBL_INSTANCE_END

//! @}

GBL_PROPERTIES(GUM_Controller,
	(controllerId,		GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(isKeyboard,		GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE)
)


GblType GUM_Controller_type(void);

// Takes a list of Name/Value pairs
#define GUM_Controller_create(...) 		GBL_NEW(GUM_Controller __VA_OPT__(,) __VA_ARGS__)

/*!
 *	Takes in a GUM_Controller,
 *	a GUM_CONTROLLER_BUTTON_STATE, and a GUM_CONTROLLER_BUTTON_ID.
 *	Handles the event.
*/
GBL_EXPORT void GUM_Controller_sendButton			(GBL_SELF, GUM_CONTROLLER_BUTTON_STATE state, GUM_CONTROLLER_BUTTON_ID button) GBL_NOEXCEPT;

//! Takes in a GUM_Controller, and sets its selected button to the passed GUM_Button
GBL_EXPORT void GUM_Controller_setSelectedButton	(GBL_SELF, GUM_Button *pButton) 											   GBL_NOEXCEPT;

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_CONTROLLER_H
