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
 *   \author    2025, 2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gimbal/gimbal_strings.h>
#include <gumball/gumball_types.h>
#include <gumball/gumball_devices.h>

GBL_DECLS_BEGIN

//! Updates the active backend.
void        GUM_Backend_update                           (void);
//! Deinitializes the active backend.
void        GUM_Backend_deinit                           (void);

//! Returns the resource type used by the active backend for pPath.
GblType     GUM_Backend_resourceType                     (GblStringRef* pPath);

//! Installs the backend logger.
void        GUM_Backend_setLogger                        (void);
//! Restores the previous logger.
void        GUM_Backend_resetLogger                      (void);

//! Returns the current timestamp in milliseconds since the program started.
uint32_t    GUM_Backend_timestamp                        (void);
//! Returns the last frame's draw time in seconds.
float       GUM_Backend_frametime                        (void);

//! Returns the current screen size.
GUM_Vector2 GUM_Backend_screenSize                       (void);
//! Draws a rectangle, optionally rounded.
GBL_RESULT  GUM_Backend_rectangleDraw                    (GUM_Renderer* pRenderer, GUM_Rectangle rectangle, float roundness, GUM_Color color);
//! Draws a rectangle outline, optionally rounded.
GBL_RESULT  GUM_Backend_rectangleLinesDraw               (GUM_Renderer* pRenderer, GUM_Rectangle rectangle, float roundness, float border_width, GUM_Color color);
//! Begins scissor mode.
GBL_RESULT  GUM_Backend_beginScissor                     (GUM_Renderer* pRenderer, GUM_Rectangle clipRect);
//! Ends scissor mode.
GBL_RESULT  GUM_Backend_endScissor                       (GUM_Renderer* pRenderer);

//! Loads a texture.
GBL_RESULT  GUM_Backend_Texture_load                     (GUM_IResource* pResource, GblStringRef* pPath);
//! Unloads a texture.
GBL_RESULT  GUM_Backend_Texture_unload                   (GUM_IResource* pResource);
//! Draws a texture.
GBL_RESULT  GUM_Backend_Texture_draw                     (GUM_Renderer* pRenderer, GUM_Texture* pTexture, GUM_Rectangle rectangle, GUM_Color color);

//! Loads a font.
GBL_RESULT   GUM_Backend_Font_load                       (GUM_IResource* pResource, GblStringRef* pPath);
//! Unloads a font.
GBL_RESULT   GUM_Backend_Font_unload                     (GUM_IResource* pResource);
//! Draws text.
GBL_RESULT   GUM_Backend_Font_draw                       (GUM_Renderer* pRenderer, GUM_Font* pFont, GblStringRef* pText, GUM_Vector2 position, GUM_Color color, int fontSize, float spacing);
//! Returns the size of the passed text for the given font and font size.
GUM_Vector2  GUM_Backend_Font_measureText                (GUM_Font* pFont, GblStringRef* pText, uint8_t fontSize);
//! Returns the default font.
GUM_Font*    GUM_Backend_Font_default                    (void);

//! Updates the mouse state.
void         GUM_Backend_Mouse_update                    (GUM_Mouse* pMouse);

//! Updates the keyboard state.
void         GUM_Backend_Keyboard_update                 (GUM_Keyboard* pKeyboard);

//! Checks whether the indexed gamepad is connected.
bool         GUM_Backend_Gamepad_isConnected             (int index);
//! Returns the indexed gamepad's name.
const char*  GUM_Backend_Gamepad_name                    (int index);
//! Updates the gamepad state.
void         GUM_Backend_Gamepad_update                  (GUM_Gamepad* pGamepad);

GBL_DECLS_END
#endif // GUM_BACKEND_H
