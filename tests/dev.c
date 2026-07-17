#include <gumball/gumball.h>
#include <raylib.h>

void buttonCallbackPress(GUM_Widget* pSelf, GUM_Event_Input* pEvent) {
    pSelf->a *= 0.6;
    printf("%s was pressed\n", GblFlags_nick(pEvent->button, GBL_TYPEID(GUM_MOUSE_FLAGS)));
    printf("action is %s\n", GblEnum_nick(pEvent->action, GBL_TYPEID(GUM_InputAction)));
}

void buttonCallbackRelease(GUM_Widget* pSelf, GUM_Event_Input* pEvent) {
    pSelf->a *= 1.6666667;
    printf("Event timestamp: %.2f seconds since startup\n", GUM_EVENT(pEvent)->timestamp / 1000.0);
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

    GblObject_foreachChild(GblObject_childFirst(GBL_OBJECT(pParentContainer)), pButton, GUM_Button*) {
        if(GblType_check(GBL_TYPEOF(pButton), GUM_BUTTON_TYPE)) {
            GUM_connect(pButton, "onPress",   buttonCallbackPress);
            GUM_connect(pButton, "onRelease", buttonCallbackRelease);
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