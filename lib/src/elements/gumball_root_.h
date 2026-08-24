#ifndef GUMBALL_ROOT_PRIVATE_H
#define GUMBALL_ROOT_PRIVATE_H

#include <gumball/elements/gumball_root.h>
#include <gumball/types/gumball_vector2.h>

GBL_FORWARD_DECLARE_STRUCT(GUM_Widget);
GBL_FORWARD_DECLARE_STRUCT(GUM_Renderer);

typedef GblBool (*GUM_Root_WidgetIterFn_)(GUM_Widget* pWidget, void* pClosure);

GUM_Root*   GUM_Root_active_           (void);
void        GUM_Root_drawEnable_       (GUM_Widget* pWidget);
void        GUM_Root_drawDisable_      (GUM_Widget* pWidget);
void        GUM_Root_drawOrderChanged_ (void);
GBL_RESULT  GUM_Root_foreachDrawable_  (GUM_Root* pRoot, GUM_Root_WidgetIterFn_ pFnIter, void* pClosure);
GBL_RESULT  GUM_Root_draw_             (GUM_Root* pRoot, GUM_Renderer* pRenderer);
GUM_Widget* GUM_Root_pointerTargetAt_  (GUM_Root* pRoot, GUM_Vector2 position);
GUM_Widget* GUM_Root_pointerHoverAt_   (GUM_Root* pRoot, GUM_Vector2 position);

#endif // GUMBALL_ROOT_PRIVATE_H
