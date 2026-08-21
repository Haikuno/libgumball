#include "gumball_backend_parity_scene.h"

GUM_Root* GUM_BackendParityScene_create(void) {
    GUM_Root* pRoot = GUM_Root_create();

    GUM_Widget_create("x", 8.0f,
                      "y", 8.0f,
                      "w", 32.0f,
                      "h", 20.0f,
                      "color", 0xC04040FF);

    GUM_Widget_create("x", 48.0f,
                      "y", 8.0f,
                      "w", 32.0f,
                      "h", 20.0f,
                      "color", 0x40B060FF);

    GUM_Widget_create("x", 88.0f,
                      "y", 8.0f,
                      "w", 24.0f,
                      "h", 20.0f,
                      "color", 0x4060C0FF);

    GUM_Container* pContainer = GUM_Container_create("x", 8.0f,
                                                     "y", 40.0f,
                                                     "w", 104.0f,
                                                     "h", 40.0f,
                                                     "padding", 4.0f,
                                                     "margin", 2.0f,
                                                     "color", 0x303038FF);

    GUM_Widget_create("parent", pContainer,
                      "color", 0xD0A040FF);
    GUM_Widget_create("parent", pContainer,
                      "color", 0xA050C0FF);

    return pRoot;
}
