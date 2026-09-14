#ifndef GUMBALL_ITEMVIEW_PRIVATE_H
#define GUMBALL_ITEMVIEW_PRIVATE_H

#include <gumball/elements/gumball_widget.h>
#include <gumball/ifaces/gumball_iitemmodel.h>
#include <gumball/types/gumball_renderer.h>

GBL_RESULT GUM_ItemView_drawText_(GUM_Widget* pWidget,
                                  GUM_Renderer* pRenderer,
                                  GUM_IItemModel* pModel,
                                  GUM_ModelIndex index,
                                  GUM_Vector2 position);

#endif // GUMBALL_ITEMVIEW_PRIVATE_H
