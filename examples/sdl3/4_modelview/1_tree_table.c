#include <gumball/gumball.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static void treeSelectionChanged_(GblInstance* pReceiver,
                                  void* pHandle,
                                  size_t column) {
    GBL_UNUSED(column);
    GUM_PropertyModel_setObject(GUM_PROPERTYMODEL(pReceiver),
                                pHandle ? GBL_OBJECT(pHandle) : nullptr);
}

static GUM_Widget* heading_(const char* pText, float x, float width) {
    return GUM_Widget_create("x", x,
                             "y", 18.0f,
                             "w", width,
                             "h", 30.0f,
                             "color", 0x00000000u,
                             "font_color", 0x202020FFu,
                             "font_size", 18,
                             "label", pText,
                             "isInteractive", false);
}

int main(int argc, char* argv[]) {
    GBL_UNUSED(argc, argv);

    int exitCode = 1;
    SDL_Window* pWindow = nullptr;
    SDL_Renderer* pSdlRenderer = nullptr;
    GUM_Renderer* pRenderer = nullptr;
    GUM_Root* pRoot = nullptr;
    GUM_ObjectTreeModel* pTreeModel = nullptr;
    GUM_PropertyModel* pPropertyModel = nullptr;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
        goto cleanup;

    pWindow = SDL_CreateWindow("Model/View - Tree + Table", 1200, 680, 0);
    if (!pWindow)
        goto cleanup;

    pSdlRenderer = SDL_CreateRenderer(pWindow, nullptr);
    if (!pSdlRenderer)
        goto cleanup;

    pRenderer = GUM_Renderer_create(pSdlRenderer);
    if (!pRenderer)
        goto cleanup;

    pRoot = GUM_Root_create();
    if (!pRoot)
        goto cleanup;

    GUM_Widget* pHierarchyHeading = heading_("Hierarchy", 20.0f, 300.0f);
    GUM_Widget* pPropertiesHeading = heading_("Properties", 340.0f, 440.0f);
    GUM_Widget* pPreviewHeading = heading_("Live preview", 800.0f, 380.0f);

    GUM_Tree* pTree = GUM_Tree_create("x", 20.0f,
                                      "y", 55.0f,
                                      "w", 300.0f,
                                      "h", 500.0f,
                                      "rowHeight", 24.0f,
                                      "indentWidth", 18.0f,
                                      "color", 0xF7F7F7FFu,
                                      "font_color", 0x202020FFu,
                                      "border_color", 0x707070FFu,
                                      "border_width", 1,
                                      "isSelectedByDefault", true);

    GUM_Table* pTable = GUM_Table_create("x", 340.0f,
                                         "y", 55.0f,
                                         "w", 440.0f,
                                         "h", 500.0f,
                                         "rowHeight", 24.0f,
                                         "color", 0xF7F7F7FFu,
                                         "font_color", 0x202020FFu,
                                         "border_color", 0x707070FFu,
                                         "border_width", 1);

    GUM_Container* pPreview = GUM_Container_create("name", "Preview",
                                                   "x", 800.0f,
                                                   "y", 55.0f,
                                                   "w", 380.0f,
                                                   "h", 500.0f,
                                                   "padding", 8.0f,
                                                   "margin", 4.0f,
                                                   "minChildSize", 0.18f,
                                                   "color", 0xEEEEEEFFu,
                                                   "border_color", 0x707070FFu,
                                                   "border_width", 1);

    GUM_Container* pToolbar = GUM_Container_create("name", "Toolbar",
                                                   "parent", pPreview,
                                                   "direction", GUM_DIRECTION_HORIZONTAL,
                                                   "padding", 6.0f,
                                                   "margin", 3.0f,
                                                   "color", 0xDCDCDCFFu,
                                                   "border_color", 0xA0A0A0FFu,
                                                   "border_width", 1);

    GUM_Button* pNew = GUM_Button_create("name", "NewButton",
                                         "parent", pToolbar,
                                         "label", "New",
                                         "color", 0xD6E8FFFFu,
                                         "font_color", 0x202020FFu);
    GUM_Button* pOpen = GUM_Button_create("name", "OpenButton",
                                          "parent", pToolbar,
                                          "label", "Open",
                                          "color", 0xE8E0FFFFu,
                                          "font_color", 0x202020FFu);
    GUM_Button* pSave = GUM_Button_create("name", "SaveButton",
                                          "parent", pToolbar,
                                          "label", "Save",
                                          "color", 0xDFF3DFFFu,
                                          "font_color", 0x202020FFu);

    GUM_Container* pContent = GUM_Container_create("name", "Content",
                                                   "parent", pPreview,
                                                   "padding", 5.0f,
                                                   "margin", 2.0f,
                                                   "minChildSize", 0.14f,
                                                   "scrollbarPolicy", GUM_SCROLLBAR_ALWAYS,
                                                   "color", 0xFAFAFAFFu,
                                                   "border_color", 0xA0A0A0FFu,
                                                   "border_width", 1);

    static const char* const rows[] = {
        "General", "Display", "Audio", "Input", "Gameplay", "Network",
        "Accessibility", "Interface", "Controls", "Graphics", "Advanced",
        "Debug", "Profiles", "About"
    };

    for (size_t i = 0; i < GBL_COUNT_OF(rows); ++i) {
        if (!GUM_Button_create("name", rows[i],
                               "parent", pContent,
                               "label", rows[i],
                               "color", (i & 1) ? 0xE9E9E9FFu : 0xF4F4F4FFu,
                               "font_color", 0x202020FFu))
            goto cleanup;
    }

    GUM_Widget* pStatus = GUM_Widget_create("name", "Status",
                                            "parent", pPreview,
                                            "label", "Ready",
                                            "color", 0xD8EFD8FFu,
                                            "font_color", 0x204020FFu,
                                            "border_color", 0xA0A0A0FFu,
                                            "border_width", 1);

    GUM_Widget* pInstructions = GUM_Widget_create(
        "x", 20.0f,
        "y", 575.0f,
        "w", 1160.0f,
        "h", 70.0f,
        "color", 0x00000000u,
        "font_color", 0x303030FFu,
        "font_size", 14,
        "label", "Select a Tree row to inspect that live object. Mouse wheel scrolls Tree/Table/Content. Click a view, then use arrow keys for navigation. Click disclosure markers to collapse/expand.",
        "isInteractive", false);

    if (!pHierarchyHeading || !pPropertiesHeading || !pPreviewHeading ||
        !pTree || !pTable || !pPreview || !pToolbar || !pNew || !pOpen ||
        !pSave || !pContent || !pStatus || !pInstructions)
        goto cleanup;

    pTreeModel = GUM_ObjectTreeModel_create(GBL_OBJECT(pPreview));
    pPropertyModel = GUM_PropertyModel_create(GBL_OBJECT(pPreview));
    if (!pTreeModel || !pPropertyModel)
        goto cleanup;

    if (!GBL_RESULT_SUCCESS(GUM_Tree_setModel(pTree, GUM_IITEMMODEL(pTreeModel))) ||
        !GBL_RESULT_SUCCESS(GUM_Table_setModel(pTable, GUM_IITEMMODEL(pPropertyModel))))
        goto cleanup;

    if (!GBL_RESULT_SUCCESS(GblSignal_connect(GBL_INSTANCE(pTree),
                                              "selectionChanged",
                                              GBL_INSTANCE(pPropertyModel),
                                              (GblFnPtr)treeSelectionChanged_,
                                              nullptr)))
        goto cleanup;

    const GUM_ModelIndex rootIndex = GUM_IItemModel_index(GUM_IITEMMODEL(pTreeModel),
                                                          0,
                                                          0,
                                                          GUM_MODEL_INDEX_INVALID);
    if (!GUM_ModelIndex_valid(rootIndex) ||
        !GBL_RESULT_SUCCESS(GUM_Tree_select(pTree, rootIndex)))
        goto cleanup;

    exitCode = 0;
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            GUM_processEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        GUM_update();

        SDL_SetRenderDrawColor(pSdlRenderer, 242, 242, 242, 255);
        SDL_RenderClear(pSdlRenderer);
        GUM_draw(pRenderer);
        SDL_RenderPresent(pSdlRenderer);
    }

cleanup:
    if (pRoot)
        GUM_unref(pRoot);
    if (pPropertyModel)
        GUM_IItemModel_unref(GUM_IITEMMODEL(pPropertyModel));
    if (pTreeModel)
        GUM_IItemModel_unref(GUM_IITEMMODEL(pTreeModel));
    if (pRenderer)
        GUM_Renderer_destroy(pRenderer);
    if (pSdlRenderer)
        SDL_DestroyRenderer(pSdlRenderer);
    if (pWindow)
        SDL_DestroyWindow(pWindow);
    SDL_Quit();
    return exitCode;
}
