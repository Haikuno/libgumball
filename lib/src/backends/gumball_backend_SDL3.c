#include <gumball/core/gumball_backend.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

struct GUM_Texture {
	SDL_Texture *texture;
};

struct GUM_Font {
	TTF_Font *font;
};

typedef struct {
	SDL_Keycode key;
	GUM_CONTROLLER_BUTTON_ID button_ui;
} KeyBinding;

struct GUM_Renderer{
	SDL_Renderer *renderer;
};

GUM_Renderer* GUM_Renderer_create(void *pRenderer) {
    GUM_Renderer* self = malloc(sizeof(GUM_Renderer));
	SDL_Renderer* pSDL_Renderer = (SDL_Renderer*)pRenderer;

    if (self != NULL) {
        self->renderer = pSDL_Renderer;
    }

    return self;
}

constexpr static KeyBinding keyBindings[] = {
	{SDLK_UP,     GUM_CONTROLLER_UP	 		},
	{SDLK_DOWN,   GUM_CONTROLLER_DOWN		},
	{SDLK_LEFT,   GUM_CONTROLLER_LEFT		},
	{SDLK_RIGHT,  GUM_CONTROLLER_RIGHT		},
	{SDLK_SPACE,  GUM_CONTROLLER_PRIMARY	},
	{SDLK_RETURN, GUM_CONTROLLER_SECONDARY	},
	{SDLK_ESCAPE, GUM_CONTROLLER_TERTIARY	},
};
constexpr size_t keyBindingCount = sizeof(keyBindings) / sizeof(KeyBinding);

GBL_RESULT GUM_Backend_pollInput(GUM_Controller *pController) {
	return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_rectangleDraw(GUM_Renderer *pRenderer, GUM_Rectangle rectangle, float roundness, GUM_Color color) {
	return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_rectangleLinesDraw(GUM_Renderer *pRenderer, GUM_Rectangle rectangle, float roundness, float border_width, GUM_Color color) {
	return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_textDraw(GUM_Renderer *pRenderer, GUM_Font *pFont, GblStringRef *pText, float x, float y, GUM_Color color) {
	return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_textureDraw(GUM_Renderer *pRenderer, GUM_Texture *pTexture, GUM_Rectangle rectangle, GUM_Color color) {
	return GBL_RESULT_SUCCESS;
}