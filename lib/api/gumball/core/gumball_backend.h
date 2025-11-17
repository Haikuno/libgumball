#ifndef GUM_BACKEND_H
#define GUM_BACKEND_H

/*! \file
 *  \brief   Abstract backend functions
 *  \ingroup core
 *
 *  \todo
 *      - Add support for audio and fonts
 * 		- Texture rotation
 *
 *  \author 	2025 Agustín Bellagamba
 *  \copyright 	MIT License
*/

#include <gimbal/gimbal_strings.h>
#include <gumball/elements/gumball_controller.h>
#include <gumball/gumball_types.h>

GBL_DECLS_BEGIN

GBL_RESULT GUM_Backend_pollInput		 	(GUM_Controller *pController);																				//!< Polls input
GBL_RESULT GUM_Backend_rectangleDraw	 	(GUM_Renderer   *pRenderer, GUM_Rectangle rectangle, float roundness, GUM_Color color);						//!< Draws a rectangle, can optionally be rounded.
GBL_RESULT GUM_Backend_rectangleLinesDraw	(GUM_Renderer   *pRenderer, GUM_Rectangle rectangle, float roundness, float border_width, GUM_Color color);	//!< Draws a rectangle outline, can optionally be rounded.
GBL_RESULT GUM_Backend_textDraw			 	(GUM_Renderer   *pRenderer, GUM_Font *pFont, GblStringRef *pText, float x, float y, GUM_Color color);		//!< Draws text
GBL_RESULT GUM_Backend_textureDraw		 	(GUM_Renderer   *pRenderer, GUM_Texture *pTexture, GUM_Rectangle rectangle, GUM_Color color);				//!< Draws a texture
void 	   GUM_Backend_setLogger			(void);																										//!< Overrides the library's logger with libGumball's one.

GBL_DECLS_END

#endif // GUM_BACKEND_H
