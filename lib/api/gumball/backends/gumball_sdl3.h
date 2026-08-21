#ifndef GUM_SDL3_H
#define GUM_SDL3_H

/*!  \file
 *   \brief SDL3 backend integration
 *
 *   \author     2026 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include <SDL3/SDL.h>
#include <gumball/types/gumball_renderer.h>

GBL_DECLS_BEGIN

//! Creates a libGumball renderer wrapper around an SDL_Renderer.
GUM_Renderer* GUM_SDL3_Renderer_create(SDL_Renderer* pRenderer);
//! Feeds an SDL event to libGumball without taking ownership of the application's event loop.
void          GUM_SDL3_processEvent(const SDL_Event* pEvent);

GBL_DECLS_END

#endif // GUM_SDL3_H
