#ifndef GUMBALL_SCROLLVIEWPORT_PRIVATE_H
#define GUMBALL_SCROLLVIEWPORT_PRIVATE_H

#include <gumball/types/gumball_animator.h>

typedef enum GUM_ScrollAxis_ {
    GUM_SCROLL_AXIS_NULL = 0,
    GUM_SCROLL_AXIS_X,
    GUM_SCROLL_AXIS_Y
} GUM_ScrollAxis_;

typedef struct GUM_ScrollViewport_ {
    GUM_Animator x;
    GUM_Animator y;
    float rangeX;
    float rangeY;
} GUM_ScrollViewport_;

void    GUM_ScrollViewport_init_        (GUM_ScrollViewport_* pSelf) GBL_NOEXCEPT;
void    GUM_ScrollViewport_reset_       (GUM_ScrollViewport_* pSelf) GBL_NOEXCEPT;
float   GUM_ScrollViewport_position_    (const GUM_ScrollViewport_* pSelf, GUM_ScrollAxis_ axis) GBL_NOEXCEPT;
float   GUM_ScrollViewport_range_       (const GUM_ScrollViewport_* pSelf, GUM_ScrollAxis_ axis) GBL_NOEXCEPT;
void    GUM_ScrollViewport_setRange_    (GUM_ScrollViewport_* pSelf, GUM_ScrollAxis_ axis, float range) GBL_NOEXCEPT;
GblBool GUM_ScrollViewport_setPosition_ (GUM_ScrollViewport_* pSelf, GUM_ScrollAxis_ axis, float position) GBL_NOEXCEPT;
void    GUM_ScrollViewport_scrollTo_    (GUM_ScrollViewport_* pSelf, GUM_ScrollAxis_ axis, float position) GBL_NOEXCEPT;
void    GUM_ScrollViewport_scrollBy_    (GUM_ScrollViewport_* pSelf, GUM_ScrollAxis_ axis, float delta) GBL_NOEXCEPT;
GblBool GUM_ScrollViewport_update_      (GUM_ScrollViewport_* pSelf, float dt) GBL_NOEXCEPT;

#endif // GUMBALL_SCROLLVIEWPORT_PRIVATE_H
