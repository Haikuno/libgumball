#ifndef GUMBALL_CONTAINER_PRIVATE_H
#define GUMBALL_CONTAINER_PRIVATE_H

#include <gumball/elements/gumball_container.h>
#include <gumball/types/gumball_animator.h>

#define GUM_CONTAINER_(self) (GBL_PRIVATE(GUM_Container, self))

typedef struct GUM_Container_ {
    float         padding;
    float         margin;
    float         minChildSize;
    float         scrollLimitX;
    float         scrollLimitY;
    GUM_Direction direction;
    bool          resizeWidgets;
    bool          alignWidgets;
    bool          scrollable;
    GUM_Animator  scrollAnimatorX;
    GUM_Animator  scrollAnimatorY;
} GUM_Container_;

void GUM_Container_scrollBy_(GUM_Container* pSelf,
                             GUM_Direction axis,
                             float delta) GBL_NOEXCEPT;
void GUM_Container_scrollTo_(GUM_Container* pSelf,
                             GUM_Direction axis,
                             float offset) GBL_NOEXCEPT;

#endif // GUMBALL_CONTAINER_PRIVATE_H
