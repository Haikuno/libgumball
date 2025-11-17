#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_logger.h>
#include <raylib.h>

struct GUM_Font {
	Font *font;
};

struct GUM_Renderer{};

GUM_Vector2 GUM_Texture_size(GUM_Texture* pSelf) {
	GUM_Vector2 size;
	Texture2D* pTexture = (Texture2D*)GblBox_field(GBL_BOX(pSelf), GblQuark_fromStatic("GUM_Texture_texture"));
	if (pTexture) {
		size.x = (float)pTexture->width;
		size.y = (float)pTexture->height;
	}
	return size;
}

static void GUM_raylibTraceLog(int logLevel, const char *text, va_list args) {
	char buf[1024];
    vsnprintf(buf, sizeof(buf), text, args);
	switch (logLevel) {
		case LOG_ALL:
		case LOG_TRACE:
		case LOG_DEBUG:
			GBL_LOG_DEBUG("raylib ", buf);
			break;
		case LOG_INFO:
			GBL_LOG_INFO("raylib ", buf);
			break;
		case LOG_WARNING:
			GBL_LOG_WARN("raylib ", buf);
			break;
		case LOG_ERROR:
		case LOG_FATAL:
			GBL_LOG_ERROR("raylib ", buf);
			break;
	}
}

void GUM_Backend_setLogger(void) {
	SetTraceLogCallback(GUM_raylibTraceLog);
}

void GUM_Texture_loadFromBytes(GUM_Texture* pSelf) {
	if (!pSelf) return;
	GblByteArray *pByteArray = *(GblByteArray**)GblBox_field(GBL_BOX(pSelf), GblQuark_fromStatic("GUM_Resource_byteArray"));
	GblStringRef *extension  = (GblStringRef*)GblBox_field(GBL_BOX(pSelf), GblQuark_fromStatic("GUM_Resource_extension"));

	Image image = LoadImageFromMemory(extension, GblByteArray_data(pByteArray), GblByteArray_size(pByteArray));
	Texture2D texture = LoadTextureFromImage(image);


	void* pTexture = malloc(sizeof(Texture2D));
	if (!pTexture) {
		UnloadImage(image);
		return;
	}
	memcpy(pTexture, &texture, sizeof(Texture2D));
	UnloadImage(image);
	GblByteArray_clear(pByteArray);
	GblBox_setField(GBL_BOX(pSelf), GblQuark_fromStatic("GUM_Texture_texture"), (uintptr_t)pTexture);
}

void GUM_Texture_unload(GUM_Texture* pSelf) {
	if (!pSelf) return;
	void* pTexture = (void*)GblBox_field(GBL_BOX(pSelf), GblQuark_fromStatic("GUM_Texture_texture"));
	UnloadTexture(*(Texture2D*)pTexture);
	free(pTexture);
}

GUM_Font *GUM_Font_create(void *pFont) {
    GUM_Font *pGUMFont = (GUM_Font *)malloc(sizeof(GUM_Font));
	if (!pGUMFont) return nullptr;

	if (pFont == NULL) {
        static Font defaultFont;
        static bool initialized = false;
        if (!initialized) {
            defaultFont = GetFontDefault();
            initialized = true;
        }
        pGUMFont->font = &defaultFont;
    } else {
        pGUMFont->font = (Font *)pFont;
    }

    return pGUMFont;
}

GUM_Vector2 GUM_Font_measureText(GUM_Font *pFont, GblStringRef *pText, uint8_t fontSize) {
	GUM_Vector2 size;
	Vector2 raySize = MeasureTextEx(*pFont->font, pText, fontSize, 1.0f);
	size.x = raySize.x;
	size.y = raySize.y;
	return size;
}

GUM_Renderer *GUM_Renderer_create(void *pRenderer) {
	return nullptr;
}

GBL_RESULT GUM_Backend_pollInput(GUM_Controller *pController) {
	typedef struct {
		KeyboardKey key;
		GUM_CONTROLLER_BUTTON_ID button_ui;
	} KeyBinding;

	typedef struct {
		GamepadButton button_gamepad;
		GUM_CONTROLLER_BUTTON_ID button_ui;
	} ButtonBinding;

	static KeyBinding constexpr key_bindings[] = {
		{ KEY_UP,		GUM_CONTROLLER_UP		},
		{ KEY_RIGHT,	GUM_CONTROLLER_RIGHT		},
		{ KEY_DOWN,		GUM_CONTROLLER_DOWN		},
		{ KEY_LEFT,		GUM_CONTROLLER_LEFT		},
        { KEY_A,		GUM_CONTROLLER_PRIMARY	},
		{ KEY_S,		GUM_CONTROLLER_SECONDARY	},
		{ KEY_D,		GUM_CONTROLLER_TERTIARY	},
    };
	static size_t constexpr n_key_bindings = sizeof(key_bindings) / sizeof(key_bindings[0]);

	static ButtonBinding constexpr button_bindings[] = {
		{ 	GAMEPAD_BUTTON_LEFT_FACE_UP, 		GUM_CONTROLLER_UP 		},
		{	GAMEPAD_BUTTON_LEFT_FACE_RIGHT, 	GUM_CONTROLLER_RIGHT 	},
		{	GAMEPAD_BUTTON_LEFT_FACE_DOWN, 		GUM_CONTROLLER_DOWN 		},
		{	GAMEPAD_BUTTON_LEFT_FACE_LEFT, 		GUM_CONTROLLER_LEFT		},
		{	GAMEPAD_BUTTON_RIGHT_FACE_DOWN, 	GUM_CONTROLLER_PRIMARY	},
		{	GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,	GUM_CONTROLLER_SECONDARY	},
		{	GAMEPAD_BUTTON_RIGHT_FACE_UP, 		GUM_CONTROLLER_TERTIARY	},
	};
	static size_t constexpr n_button_bindings = sizeof(button_bindings) / sizeof(button_bindings[0]);

	if (pController->isKeyboard) {
		for (size_t i = 0; i < n_key_bindings; ++i) {
			if (IsKeyPressed(key_bindings[i].key))
				GUM_Controller_sendButton(pController, GUM_CONTROLLER_BUTTON_PRESS, key_bindings[i].button_ui);
			if (IsKeyReleased(key_bindings[i].key))
				GUM_Controller_sendButton(pController, GUM_CONTROLLER_BUTTON_RELEASE, key_bindings[i].button_ui);
		}

		return GBL_RESULT_SUCCESS;
	}

	for (size_t i = 0; i < n_button_bindings; ++i) {
		if (IsGamepadButtonPressed(pController->controllerId, button_bindings[i].button_gamepad))
			GUM_Controller_sendButton(pController, GUM_CONTROLLER_BUTTON_PRESS, button_bindings[i].button_ui);
		if (IsGamepadButtonReleased(pController->controllerId, button_bindings[i].button_gamepad))
			GUM_Controller_sendButton(pController, GUM_CONTROLLER_BUTTON_RELEASE, button_bindings[i].button_ui);
	}

	return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_rectangleDraw(GUM_Renderer *pRenderer, GUM_Rectangle rectangle, float roundness, GUM_Color color) {
	DrawRectangleRounded((Rectangle){ rectangle.x, rectangle.y, rectangle.width, rectangle.height }, roundness, 4, (Color){ color.r, color.g, color.b, color.a });
	return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_rectangleLinesDraw(GUM_Renderer *pRenderer, GUM_Rectangle rectangle, float roundness, float border_width, GUM_Color color) {
	DrawRectangleRoundedLinesEx((Rectangle){ rectangle.x, rectangle.y, rectangle.width, rectangle.height }, roundness, 4, border_width, (Color){ color.r, color.g, color.b, color.a });
	return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_textDraw(GUM_Renderer *pRenderer, GUM_Font *pFont, GblStringRef *pText, float x, float y, GUM_Color color) {
	DrawTextEx(*pFont->font, pText, (Vector2){ x, y }, 22, 1.2f, (Color){ color.r, color.g, color.b, color.a });
	return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_textureDraw(GUM_Renderer *pRenderer, GUM_Texture *pTexture, GUM_Rectangle rectangle, GUM_Color color) {
	if (!pTexture) return GBL_RESULT_ERROR_INVALID_POINTER;
	Texture2D texture = *(Texture2D*)GblBox_field(GBL_BOX(pTexture), GblQuark_fromStatic("GUM_Texture_texture"));
	Rectangle src = { 0, 0, (float)texture.width, (float)texture.height };
	Rectangle dst = { rectangle.x, rectangle.y, rectangle.width, rectangle.height };
	DrawTexturePro(texture, src, dst, (Vector2){ 0, 0 }, 0.0f, (Color){ 255, 255, 255, 255 });
	return GBL_RESULT_SUCCESS;
}
