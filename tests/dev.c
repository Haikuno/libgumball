#include <gumball/gumball.h>
#include <raylib.h>

void buttonOnPress(GUM_Widget* pSelf, GUM_Event_Input* pEvent) {
    pSelf->r -= 10;
    pSelf->g -= 10;
    pSelf->b -= 10;
}

void buttonOnRelease(GUM_Widget* pSelf, GUM_Event_Input* pEvent) {
    pSelf->r += 10;
    pSelf->g += 10;
    pSelf->b += 10;
}

void widgetOnFocusGained(GUM_Widget* pSelf, GUM_InputDevice* pInputDevice) {
    pSelf->border_r = 255;
    pSelf->border_g = 255;
    pSelf->border_b = 255;
    pSelf->border_a = 255;
}

void widgetOnFocusLost(GUM_Widget* pSelf, GUM_InputDevice* pInputDevice) {
    pSelf->border_r  = 0;
    pSelf->border_g  = 0;
    pSelf->border_b  = 0;
    pSelf->border_a  = 50;
}


int main(int argc, char* argv[]) {
    GUM_Root* pRoot = GUM_Root_create();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(640, 480, "Test");
    SetTargetFPS(60);

    GUM_Container* pParentContainer = GUM_Container_create(
        "x", 0.0f,  "y", 0.0f,
        "color", 0,  "border_color", 0x000000AF,
        "margin", 10.0f, "children",
        GblRingList_create(

            GUM_Container_create(
                "border_color", 0x000000FF, "orientation", 'h',
                "color", 0xF0F000FF,
                "margin", 5.0f, "padding", 10.0f, "children",
                GblRingList_create(
                    GUM_Button_create("color", 0xFF0000FF, "label", "I'm red!",   "border_color", 0xA0A000FF),
                    GUM_Button_create("color", 0x0000FFFF, "label", "I'm blue!",  "border_color", 0xA0A000FF),
                    GUM_Button_create("color", 0x00FF00FF, "label", "I'm green!", "border_color", 0xA0A000FF)
                )
            ),

            GUM_Container_create(
                "border_color", 0x000000FF, "orientation", 'v',
                "color", 0X0F0F0FFF,
                "margin", 3.0f, "children",
                GblRingList_create(
                    GUM_Button_create("color", 0xFFFF00FF, "label", "I'm yellow!",  "font_color", 0x000000FF),
                    GUM_Button_create("color", 0xFF00FFFF, "label", "I'm magenta!", "font_color", 0x000000FF),
                    GUM_Button_create("color", 0x00FFFFFF, "label", "I'm cyan!",    "font_color", 0x000000FF)
                )
            )
        )
    );

    GblObject_foreachChild(GblObject_childFirst(GBL_OBJECT(pParentContainer)), pChild) {
        if (GBL_TYPECHECK(GUM_Widget, pChild)) {
            GUM_connect(pChild, "onFocusGained",    widgetOnFocusGained);
            GUM_connect(pChild, "onFocusLost",      widgetOnFocusLost);
            GUM_connect(pChild, "onPressConfirm",   buttonOnPress);
            GUM_connect(pChild, "onReleaseConfirm", buttonOnRelease);
        }
    }

    GblObject_foreachChild(GblObject_siblingNext(GblObject_childFirst(GBL_OBJECT(pParentContainer))), pChild) {
        if (GBL_TYPECHECK(GUM_Widget, pChild)) {
            GUM_connect(pChild, "onFocusGained",    widgetOnFocusGained);
            GUM_connect(pChild, "onFocusLost",      widgetOnFocusLost);
            GUM_connect(pChild, "onPressConfirm",   buttonOnPress);
            GUM_connect(pChild, "onReleaseConfirm", buttonOnRelease);
        }
    }

    GUM_InputSystem_bind(GUM_MOUSE_TYPE, GUM_INPUTACTION_CONFIRM, GUM_MOUSE_BUTTON_MIDDLE);

    while (!WindowShouldClose()) {
        pParentContainer->base.w = GetScreenWidth();
        pParentContainer->base.h = GetScreenHeight();
        GUM_update();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        GUM_draw();
        EndDrawing();
    }

    GUM_unref(pRoot);
    CloseWindow();
    return 0;
}