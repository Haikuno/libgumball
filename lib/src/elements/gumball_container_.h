#ifndef GUMBALL_CONTAINER_PRIVATE_H
#define GUMBALL_CONTAINER_PRIVATE_H

#include <gumball/elements/gumball_container.h>

#include "gumball_scrollviewport_.h"

#define GUM_CONTAINER_(self) (GBL_PRIVATE(GUM_Container, self))

typedef struct GUM_Container_ {
    float                padding;
    float                margin;
    float                minChildSize;
    float                scrollbarThickness;
    float                scrollbarGap;
    float                scrollbarMinThumbSize;
    float                scrollbarRoundness;
    uint32_t             scrollbarTrackColor;
    uint32_t             scrollbarThumbColor;
    float                scrollbarGrabOffset;
    GUM_Direction        direction;
    GUM_ScrollbarPolicy scrollbarPolicy;
    bool                 resizeWidgets;
    bool                 alignWidgets;
    bool                 scrollable;
    bool                 scrollbarDragging;
    GUM_ScrollViewport_  viewport;
} GUM_Container_;

void    GUM_Container_scrollBy_          (GUM_Container* pSelf,
                                          GUM_Direction axis,
                                          float delta) GBL_NOEXCEPT;
void    GUM_Container_scrollTo_          (GUM_Container* pSelf,
                                          GUM_Direction axis,
                                          float offset) GBL_NOEXCEPT;
GblBool GUM_Container_scrollbarGeometry_ (const GUM_Container* pSelf,
                                          GUM_Rectangle* pTrack,
                                          GUM_Rectangle* pThumb) GBL_NOEXCEPT;

#endif // GUMBALL_CONTAINER_PRIVATE_H
