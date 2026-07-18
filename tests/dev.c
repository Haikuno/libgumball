#include <gumball/gumball.h>
#include <raylib.h>

void onPressCallback(GUM_Widget* pSelf, GUM_Event_Input* pEvent) {
    pSelf->r -= 10;
    pSelf->g -= 10;
    pSelf->b -= 10;
}

void onReleaseCallback(GUM_Widget* pSelf, GUM_Event_Input* pEvent) {
    pSelf->r += 10;
    pSelf->g += 10;
    pSelf->b += 10;
}

int main(int argc, char* argv[]) {
    GUM_Root* pRoot = GUM_Root_create();

    InitWindow(640, 480, "Test");
    SetTargetFPS(60);

    GUM_Container* pParentContainer = GUM_Container_create(
        "x", 0.0f,   "y", 0.0f,
        "w", 640.0f, "h", 480.0f,
        "color", 0x0F0F0FFF, "border_color", 0x000000AF,
        "margin", 10.0f, "children",
        GblRingList_create(

            GUM_Container_create(
                "border_color", 0x000000FF, "orientation", 'h',
                "color", 0x0F0F0FFF,
                "margin", 10.0f, "padding", 10.0f, "children",
                GblRingList_create(
                    GUM_Button_create("color", 0x0A0A0AFF, "label", "Click me!"),
                    GUM_Button_create("color", 0x0A0A0AFF, "label", "Click me!"),
                    GUM_Button_create("color", 0x0A0A0AFF, "label", "Click me!")
                )
            ),

            GUM_Container_create(
                "border_color", 0x000000FF, "orientation", 'v',
                "color", 0x0F0F0FFF,
                "margin", 10.0f, "children",
                GblRingList_create(
                    GUM_Button_create("color", 0x0A0A0AFF, "label", "Click me!"),
                    GUM_Button_create("color", 0x0A0A0AFF, "label", "Click me!"),
                    GUM_Button_create("color", 0x0A0A0AFF, "label", "Click me!")
                )
            )
        )
    );

    GblObject_foreachChild(GblObject_childFirst(GBL_OBJECT(pParentContainer)), pChild) {
        GUM_connect(pChild, "onPressConfirm",   onPressCallback);
        GUM_connect(pChild, "onReleaseConfirm", onReleaseCallback);
    }

    GblObject_foreachChild(GblObject_siblingNext(GblObject_childFirst(GBL_OBJECT(pParentContainer))), pChild) {
        GUM_connect(pChild, "onPressConfirm",   onPressCallback);
        GUM_connect(pChild, "onReleaseConfirm", onReleaseCallback);
    }

    while (!WindowShouldClose()) {
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