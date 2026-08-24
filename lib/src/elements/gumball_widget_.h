#ifndef GUMBALL_WIDGET_PRIVATE_H
#define GUMBALL_WIDGET_PRIVATE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <gumball/elements/gumball_widget.h>

#define GUM_WIDGET_(self) (GBL_PRIVATE(GUM_Widget, self))

constexpr GUM_Rectangle GUM_CLIP_RECT_NONE_ = ((GUM_Rectangle){ -1e30f, -1e30f, 2e30f, 2e30f });

typedef struct GUM_Widget_ {
    GUM_Widget*   pDrawPrev;
    GUM_Widget*   pDrawNext;
    GblStringRef* pLabel;
    GUM_Font*     pFont;
    GUM_Texture*  pTexture;
    GUM_Rectangle clipRect;
    uint64_t      enableOrder;
    uint8_t       zIndex;
    size_t        focusCount;
    bool          drawEnabled;
    bool          drawMember;
    bool          active;
    bool          activeTransition;
} GUM_Widget_;

GUM_Vector2   GUM_get_absolute_position_          (GUM_Widget* pWidget) GBL_NOEXCEPT;
void          GUM_Widget_animate_update_           (void) GBL_NOEXCEPT;
void          GUM_Widget_animate_widgetDestroyed_ (GUM_Widget* pWidget) GBL_NOEXCEPT;
GBL_RESULT    GUM_Widget_drawSetEnabled_           (GUM_Widget* pWidget, bool enabled) GBL_NOEXCEPT;
GBL_RESULT    GUM_Widget_hierarchyChanged_         (GUM_Widget* pWidget,
                                                    GblObject* pOldParent,
                                                    GblObject* pNewParent) GBL_NOEXCEPT;
GUM_Rectangle GUM_Widget_clipRect_                 (const GUM_Widget* pWidget) GBL_NOEXCEPT;
void          GUM_Widget_setClipRect_              (GUM_Widget* pWidget, GUM_Rectangle clip) GBL_NOEXCEPT;
void          GUM_Widget_focusAcquire_             (GUM_Widget* pWidget) GBL_NOEXCEPT;
void          GUM_Widget_focusRelease_             (GUM_Widget* pWidget) GBL_NOEXCEPT;
void          GUM_Widget_initActive_               (GUM_Widget* pWidget, bool active) GBL_NOEXCEPT;

#endif // GUMBALL_WIDGET_PRIVATE_H
