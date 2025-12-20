#ifndef GUM_BACKEND_H
#define GUM_BACKEND_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__backend_8h.html

/*!  \file
 *   \brief      Abstract backend functions
 *   \ingroup    core
 *
 *   \todo
 *       - Add support for audio
 *       - Texture rotation
 *
 *   \author        2025 Agustín Bellagamba
 *   \copyright     MIT License
*/

#include <gimbal/gimbal_strings.h>
#include <gumball/elements/gumball_controller.h>
#include <gumball/gumball_types.h>

GBL_DECLS_BEGIN

//! Polls input
GBL_RESULT GUM_Backend_pollInput                        (GUM_Controller* pController);
//! Draws a rectangle, can optionally berounded
GBL_RESULT GUM_Backend_rectangleDraw                    (GUM_Renderer*   pRenderer, GUM_Rectangle rectangle, float roundness, GUM_Color color);
//! Draws a rectangle outline, can optionally be rounded
GBL_RESULT GUM_Backend_rectangleLinesDraw               (GUM_Renderer*   pRenderer, GUM_Rectangle rectangle, float roundness, float border_width, GUM_Color color);
//! Overrides the library's logger with libGumball's logger
void       GUM_Backend_setLogger                        (void);
//! Resets the library's logger back to the default
void       GUM_Backend_resetLogger                      (void);
//! Begins scissor mode
GBL_RESULT GUM_Backend_beginScissor                     (GUM_Renderer* pRenderer, GUM_Rectangle clipRect);
//! Ends scissor mode
GBL_RESULT GUM_Backend_endScissor                       (GUM_Renderer* pRenderer);

//! Loads a texture
GBL_RESULT GUM_Backend_Texture_load                     (GUM_IResource* pResource, GblStringRef* pPath);
//! Unloads a texture
GBL_RESULT GUM_Backend_Texture_unload                   (GUM_IResource* pResource);
//! Draws a texture
GBL_RESULT GUM_Backend_Texture_draw                     (GUM_Renderer*  pRenderer, GUM_Texture* pTexture, GUM_Rectangle rectangle, GUM_Color color);

//! Loads a font
GBL_RESULT GUM_Backend_Font_load                        (GUM_IResource* pResource, GblStringRef* pPath);
//! Unloads a font
GBL_RESULT GUM_Backend_Font_unload                      (GUM_IResource* pResource);
//! Draws text
GBL_RESULT GUM_Backend_Font_draw                        (GUM_Renderer*  pRenderer, GUM_Font* pFont, GblStringRef* pText, GUM_Vector2 position, GUM_Color color, int fontSize, float spacing);
//! Measures text size given a font, text and font size
GBL_RESULT GUM_Backend_Font_measureText                 (GUM_Font*      pFont, GblStringRef* pText, uint8_t fontSize);
//! Returns a pointer to the default font
GUM_Font*  GUM_Backend_Font_default                     (void);

//! Returns the current timestamp in miliseconds since the program started
uint32_t   GUM_Backend_timestamp                        (void);

GBL_DECLS_END

#endif // GUM_BACKEND_H
