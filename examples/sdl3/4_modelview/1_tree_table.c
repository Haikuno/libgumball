#include <gumball/gumball.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static void treeSelectionChanged_(GblInstance* pReceiver, void* pHandle, size_t column) {
    GBL_UNUSED(column);
    GUM_PropertyModel_setObject(GUM_PROPERTYMODEL(pReceiver),
                                pHandle ? GBL_OBJECT(pHandle) : nullptr);
}

int main(int argc, char* argv[]) {
    GBL_UNUSED(argc, argv);
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) return 1;

    SDL_Window* pWindow = SDL_CreateWindow("Model/View - Tree + Table", 1300, 600, 0);
    SDL_Renderer* pSdlRenderer = pWindow ? SDL_CreateRenderer(pWindow, nullptr) : nullptr;
    GUM_Renderer* pRenderer = pSdlRenderer ? GUM_Renderer_create(pSdlRenderer) : nullptr;
    if (!pRenderer) return 1;

    GUM_Root* pRoot = GUM_Root_create();

    GUM_Widget_create("x", 20.0f, "y", 18.0f,
                      "w", 300.0f, "h", 30.0f,
                      "color", 0x00000000u,
                      "font_color", 0x202020FFu,
                      "font_size", 18,
                      "label", "Hierarchy",
                      "isInteractive", false);

    GUM_Widget_create("x", 360.0f, "y", 18.0f,
                      "w", 440.0f, "h", 30.0f,
                      "color", 0x00000000u,
                      "font_color", 0x202020FFu,
                      "font_size", 18,
                      "label", "Properties",
                      "isInteractive", false);

    GUM_Widget_create("x", 900.0f, "y", 18.0f,
                      "w", 380.0f, "h", 30.0f,
                      "color", 0x00000000u,
                      "font_color", 0x202020FFu,
                      "font_size", 18,
                      "label", "Live preview",
                      "isInteractive", false);

    GUM_Tree* pTree = GUM_Tree_create("x", 20.0f, "y", 55.0f,
                                      "w", 300.0f, "h", 500.0f,
                                      "rowHeight", 24.0f,
                                      "indentWidth", 18.0f,
                                      "color", 0xF7F7F7FFu,
                                      "font_color", 0x202020FFu,
                                      "border_color", 0x707070FFu,
                                      "border_width", 1,
                                      "isSelectedByDefault", true);

    GUM_Table* pTable = GUM_Table_create("x", 350.0f, "y", 55.0f,
                                         "w", 520.0f, "h", 500.0f,
                                         "rowHeight", 24.0f,
                                         "color", 0xF7F7F7FFu,
                                         "font_color", 0x202020FFu,
                                         "border_color", 0x707070FFu,
                                         "border_width", 1);

    GUM_Container* pPreview = GUM_Container_create("name", "Preview",
                                                   "x", 900.0f, "y", 55.0f,
                                                   "w", 380.0f, "h", 500.0f,
                                                   "padding", 8.0f,
                                                   "margin", 4.0f,
                                                   "minChildSize", 0.18f,
                                                   "color", 0xEEEEEEFFu,
                                                   "border_color", 0x707070FFu,
                                                   "border_width", 1);

    GUM_Widget_create("name", "Title",
                      "parent", pPreview,
                      "label", "Settings preview",
                      "color", 0xDCDCDCFFu,
                      "font_color", 0x202020FFu,
                      "border_color", 0xA0A0A0FFu,
                      "border_width", 1,
                      "isInteractive", false);

    GUM_Container* pContent = GUM_Container_create("name", "Content",
                                                   "parent", pPreview,
                                                   "padding", 5.0f,
                                                   "margin", 2.0f,
                                                   "minChildSize", 0.14f,
                                                   "scrollbarPolicy", GUM_SCROLLBAR_ALWAYS,
                                                   "color", 0xFAFAFAFFu,
                                                   "border_color", 0xA0A0A0FFu,
                                                   "border_width", 1);

    const char* rows[] = {
        "General", "Display", "Audio", "Input", "Gameplay", "Network",
        "Accessibility", "Interface", "Controls", "Graphics", "Advanced",
        "Debug", "Profiles", "About"
    };

    for (size_t i = 0; i < GBL_COUNT_OF(rows); ++i)
        GUM_Widget_create("name", rows[i],
                          "parent", pContent,
                          "label", rows[i],
                          "color", (i & 1) ? 0xE9E9E9FFu : 0xF4F4F4FFu,
                          "font_color", 0x202020FFu,
                          "isInteractive", false);

    GUM_Widget_create("name", "Status",
                      "parent", pPreview,
                      "label", "Ready",
                      "color", 0xD8EFD8FFu,
                      "font_color", 0x204020FFu,
                      "border_color", 0xA0A0A0FFu,
                      "border_width", 1,
                      "isInteractive", false);

    GUM_ObjectTreeModel* pTreeModel = GUM_ObjectTreeModel_create(GBL_OBJECT(pPreview));
    GUM_PropertyModel* pPropertyModel = GUM_PropertyModel_create(GBL_OBJECT(pPreview));
    if (!pTreeModel || !pPropertyModel) return 1;

    GUM_Tree_setModel(pTree, GUM_IITEMMODEL(pTreeModel));
    GUM_Table_setModel(pTable, GUM_IITEMMODEL(pPropertyModel));
    GblSignal_connect(GBL_INSTANCE(pTree),
                      "selectionChanged",
                      GBL_INSTANCE(pPropertyModel),
                      (GblFnPtr)treeSelectionChanged_,
                      nullptr);

    GUM_Tree_select(pTree,
                    GUM_IItemModel_index(GUM_IITEMMODEL(pTreeModel),
                                         0,
                                         0,
                                         GUM_MODEL_INDEX_INVALID));

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            GUM_processEvent(&event);
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        GUM_update();

        SDL_SetRenderDrawColor(pSdlRenderer, 242, 242, 242, 255);
        SDL_RenderClear(pSdlRenderer);
        GUM_draw(pRenderer);
        SDL_RenderPresent(pSdlRenderer);
    }

    GUM_IItemModel_unref(GUM_IITEMMODEL(pPropertyModel));
    GUM_IItemModel_unref(GUM_IITEMMODEL(pTreeModel));
    GUM_unref(pRoot);
    GUM_Renderer_destroy(pRenderer);
    SDL_DestroyRenderer(pSdlRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
    return 0;
}
