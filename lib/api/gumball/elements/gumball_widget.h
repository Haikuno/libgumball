#ifndef GUM_WIDGET_H
#define GUM_WIDGET_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__widget_8h.html

/*! \file
 *  \ref     GUM_Widget "GUM_Widget data structure and hierarchy graph"
 *  \ingroup elements
 *
 * 	GUM_Widget is the most basic, fundamental element.
 *
 *	It is the parent class for all drawable elements in libGumball,
 *	holding variables that are common to all drawable elements, such as position, size, and color.
 *
 *  \todo
 * 		- Make variables private
 * 		- Separate isRelative into position and size relative
 * 		- Add GUM_Vector2 position and size as properties
 * 		- Make border highlight configurable (color, thickness)
 *
 * 	\author 2025 Agustín Bellagamba
 *	\copyright MIT License
*/

#include <gimbal/gimbal_meta.h>
#include <gimbal/gimbal_strings.h>
#include <gimbal/gimbal_core.h>

#include <gumball/types/gumball_font.h>
#include <gumball/types/gumball_texture.h>
#include <gumball/types/gumball_renderer.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_WIDGET_TYPE				(GBL_TYPEID		(GUM_Widget)) 			//!< Returns the GUM_Widget Type UUID
#define GUM_WIDGET(self)			(GBL_CAST		(GUM_Widget, self)) 	//!< Casts an instance of a compatible element to a GUM_Widget
#define GUM_WIDGET_CLASS(klass)		(GBL_CLASS_CAST (GUM_Widget, klass))	//!< Casts an class of a compatible element to a GUM_WidgetClass
#define GUM_WIDGET_CLASSOF(self)	(GBL_CLASSOF	(GUM_Widget, self))		//!< Casts an instance of a compatible element to a GUM_Widget
//! @}

#define GBL_SELF_TYPE 				 GUM_Widget

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Widget);

/*!
 *	\struct  GUM_WidgetClass
 *	\extends GblObjectClass
 *	\brief   GUM_Widget structure
 *
 *	GUM_WidgetClass derives from GblObjectClass,
 *	adding additional virtual functions to handle activating, deactivating, updating, and drawing elements.
*/
//! \cond
GBL_CLASS_DERIVE(GUM_Widget, GblObject)
	GBL_RESULT (*pFnActivate)	(GBL_SELF);
	GBL_RESULT (*pFnDeactivate)	(GBL_SELF);
	GBL_RESULT (*pFnUpdate)		(GBL_SELF);
	GBL_RESULT (*pFnDraw)		(GBL_SELF, GUM_Renderer *pRenderer);
GBL_CLASS_END
//! \endcond

/*!
 *	\class   GUM_Widget
 *	\extends GblObject
 *	\brief   Basic widget element
*/

/*!
 *	\name  Properties
 *	\brief Widget properties you can set/get at or after creation.
 *	@{
*/
//! \cond
GBL_INSTANCE_DERIVE(GUM_Widget, GblObject)
//! \endcond
	float   			x; 				  		//!< Horizontal position of the widget. 								   			Default value is 0
	float				y; 				  		//!< Vertical position of the widget. 								   				Default value is 0
	float				w; 				  		//!< Width of the widget. 											   				Default value is 200
	float				h; 				  		//!< Height of the widget. 											   				Default value is 200
	uint8_t 			r; 				  		//!< Red component of the widget color. 								   			Default value is 0
	uint8_t 		 	g; 				  		//!< Green component of the widget color. 							   				Default value is 255
	uint8_t 			b; 				  		//!< Blue component of the widget color. 								   			Default value is 0
	uint8_t 			a; 				  		//!< Alpha component of the widget color. 							   				Default value is 255
	uint8_t 			border_r;		  		//!< Red component of the border color. 								   			Default value is 0
	uint8_t 		 	border_g; 		  		//!< Green component of the border color. 							   				Default value is 0
	uint8_t 			border_b; 		  		//!< Blue component of the border color. 								   			Default value is 0
	uint8_t 			border_a; 		  		//!< Alpha component of the border color. 							   				Default value is 0
	uint8_t 			border_width;	  		//!< Width of the border, in pixels.									   			Default value is 4
	float				border_radius;	  		//!< Radius of the border.											   				Default value is 0
	bool				border_highlight; 		//!< If the border should be highlighted. 							   				Default value is false
	bool				isRelative;	      		//!< If the widget's position and size should be relative to its parent. 			Default value is false
	GblStringRef 		*label;			  		//!< Optional text label of the widget. 							   				Default value is nullptr
	GUM_Font			*font;			  		//!< Optional font for the widget's label. If not set, the default font is used.	Default value is nullptr
	GUM_TextAlignment	textAlignment;    		//!< Alignment of the widget's label. 								   				Default value is GUM_TEXT_ALIGN_CENTER		\bug This is not working at the moment.
	GUM_Texture			*texture;		  		//!< Optional texture for rendering inside the widget.				   				Default value is nullptr
	uint8_t  			font_size;		  		//!< Font size of the widget's label. 								   				Default value is 22
	uint8_t				font_r;			  		//!< Red component of the font color. 								   				Default value is 255
	uint8_t			    font_g;			  		//!< Green component of the font color. 							   				Default value is 255
	uint8_t 			font_b;			  		//!< Blue component of the font color. 								   				Default value is 255
	uint8_t				font_a;			  		//!< Alpha component of the font color. 							   				Default value is 255
	uint8_t				font_border_r;  		//!< Red component of the font border color. 						   				Default value is 0
	uint8_t				font_border_g;	  	 	//!< Green component of the font border color.						   				Default value is 0
	uint8_t				font_border_b; 			//!< Blue component of the font border color. 						   				Default value is 0
	uint8_t				font_border_a;	 	 	//!< Alpha component of the font border color.						   				Default value is 0
	uint8_t				font_border_thickness;	//!< Width of the font border, in pixels. 						   					Default value is 1
	uint8_t				z_index;			  	//!< Z-index of the widget. The higher the value, the higher the priority.			Default value is 50
GBL_INSTANCE_END
//! @}

GBL_PROPERTIES(GUM_Widget,
	(x,						GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE),
	(y,						GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE),
	(w,						GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE),
	(h,						GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE),
	(isRelative,			GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE),
	(color,					GBL_GENERIC, (READ, WRITE), GBL_UINT32_TYPE),
	(border_color,			GBL_GENERIC, (READ, WRITE), GBL_UINT32_TYPE),
	(font_border_color,		GBL_GENERIC, (READ, WRITE), GBL_UINT32_TYPE),
	(r,						GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(g,						GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(b,						GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(a,						GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(border_r,				GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(border_g,				GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(border_b,				GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(border_a,				GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(border_width,			GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(border_radius,			GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE),
	(border_highlight,		GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE),
	(label,					GBL_GENERIC, (READ, WRITE), GBL_STRING_TYPE),
	(texture,				GBL_GENERIC, (READ, WRITE), GUM_TEXTURE_TYPE),
	(textAlignment,			GBL_GENERIC, (READ, WRITE), GUM_TEXT_ALIGNMENT_TYPE),
	(font,					GBL_GENERIC, (READ, WRITE), GUM_FONT_TYPE),
	(font_size,				GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(font_r,				GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(font_g,				GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(font_b,				GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(font_a,				GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(font_border_r,			GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(font_border_g,			GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(font_border_b,			GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(font_border_a,			GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(font_border_thickness, GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
	(z_index,				GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE)
)

GblType GUM_Widget_type(void);

//! \cond
GUM_Vector2	GUM_get_absolute_position_(GBL_SELF);
//! \endcond

//! Returns a new GUM_Widget. Optionally takes in a list of Name/Value pairs for properties
#define GUM_Widget_create(/* property_name, property_value */...) GBL_NEW(GUM_Widget __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_WIDGET_H
