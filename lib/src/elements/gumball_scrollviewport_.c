#include "gumball_scrollviewport_.h"

static GUM_Animator* GUM_ScrollViewport_animator_(GUM_ScrollViewport_* pSelf,
                                                   GUM_ScrollAxis_ axis) {
    if (!pSelf)
        return nullptr;

    switch (axis) {
        case GUM_SCROLL_AXIS_X: return &pSelf->x;
        case GUM_SCROLL_AXIS_Y: return &pSelf->y;
        default:                return nullptr;
    }
}

static const GUM_Animator* GUM_ScrollViewport_animatorConst_(const GUM_ScrollViewport_* pSelf,
                                                             GUM_ScrollAxis_ axis) {
    if (!pSelf)
        return nullptr;

    switch (axis) {
        case GUM_SCROLL_AXIS_X: return &pSelf->x;
        case GUM_SCROLL_AXIS_Y: return &pSelf->y;
        default:                return nullptr;
    }
}

void GUM_ScrollViewport_init_(GUM_ScrollViewport_* pSelf) {
    if (!pSelf)
        return;

    pSelf->x      = GUM_Animator_make(0.0f, 0.2f, GUM_EASE_QUAD_OUT);
    pSelf->y      = GUM_Animator_make(0.0f, 0.2f, GUM_EASE_QUAD_OUT);
    pSelf->rangeX = 0.0f;
    pSelf->rangeY = 0.0f;
}

void GUM_ScrollViewport_reset_(GUM_ScrollViewport_* pSelf) {
    if (!pSelf)
        return;

    pSelf->rangeX = 0.0f;
    pSelf->rangeY = 0.0f;
    pSelf->x.from = pSelf->x.to = pSelf->x.current = 0.0f;
    pSelf->y.from = pSelf->y.to = pSelf->y.current = 0.0f;
    pSelf->x.elapsed = pSelf->x.duration;
    pSelf->y.elapsed = pSelf->y.duration;
}

float GUM_ScrollViewport_position_(const GUM_ScrollViewport_* pSelf,
                                   GUM_ScrollAxis_ axis) {
    const GUM_Animator* pAnimator = GUM_ScrollViewport_animatorConst_(pSelf, axis);
    return pAnimator ? pAnimator->current : 0.0f;
}

float GUM_ScrollViewport_range_(const GUM_ScrollViewport_* pSelf,
                                GUM_ScrollAxis_ axis) {
    if (!pSelf)
        return 0.0f;

    switch (axis) {
        case GUM_SCROLL_AXIS_X: return pSelf->rangeX;
        case GUM_SCROLL_AXIS_Y: return pSelf->rangeY;
        default:                return 0.0f;
    }
}

void GUM_ScrollViewport_setRange_(GUM_ScrollViewport_* pSelf,
                                  GUM_ScrollAxis_ axis,
                                  float range) {
    if (!pSelf)
        return;

    range = GBL_MAX(range, 0.0f);

    GUM_Animator* pAnimator = GUM_ScrollViewport_animator_(pSelf, axis);
    if (!pAnimator)
        return;

    switch (axis) {
        case GUM_SCROLL_AXIS_X: pSelf->rangeX = range; break;
        case GUM_SCROLL_AXIS_Y: pSelf->rangeY = range; break;
        default: return;
    }

    const float from    = GBL_CLAMP(pAnimator->from,    0.0f, range);
    const float to      = GBL_CLAMP(pAnimator->to,      0.0f, range);
    const float current = GBL_CLAMP(pAnimator->current, 0.0f, range);
    if (from == pAnimator->from && to == pAnimator->to && current == pAnimator->current)
        return;

    pAnimator->current = current;
    if (current == to) {
        pAnimator->from = pAnimator->to = current;
        pAnimator->elapsed = pAnimator->duration;
    } else {
        GUM_Animator_set(pAnimator, to);
    }
}

GblBool GUM_ScrollViewport_setPosition_(GUM_ScrollViewport_* pSelf,
                                        GUM_ScrollAxis_ axis,
                                        float position) {
    GUM_Animator* pAnimator = GUM_ScrollViewport_animator_(pSelf, axis);
    if (!pAnimator)
        return GBL_FALSE;

    const float target = GBL_CLAMP(position, 0.0f, GUM_ScrollViewport_range_(pSelf, axis));
    const bool changed = pAnimator->current != target || pAnimator->to != target || pAnimator->from != target;

    pAnimator->from    = target;
    pAnimator->to      = target;
    pAnimator->current = target;
    pAnimator->elapsed = pAnimator->duration;
    return changed;
}

void GUM_ScrollViewport_scrollTo_(GUM_ScrollViewport_* pSelf,
                                  GUM_ScrollAxis_ axis,
                                  float position) {
    GUM_Animator* pAnimator = GUM_ScrollViewport_animator_(pSelf, axis);
    if (!pAnimator)
        return;

    const float target = GBL_CLAMP(position, 0.0f, GUM_ScrollViewport_range_(pSelf, axis));
    if (pAnimator->to != target)
        GUM_Animator_set(pAnimator, target);
}

void GUM_ScrollViewport_scrollBy_(GUM_ScrollViewport_* pSelf,
                                  GUM_ScrollAxis_ axis,
                                  float delta) {
    if (!pSelf || delta == 0.0f)
        return;

    GUM_Animator* pAnimator = GUM_ScrollViewport_animator_(pSelf, axis);
    if (pAnimator)
        GUM_ScrollViewport_scrollTo_(pSelf, axis, pAnimator->to + delta);
}

GblBool GUM_ScrollViewport_update_(GUM_ScrollViewport_* pSelf, float dt) {
    if (!pSelf)
        return GBL_FALSE;

    const bool changedX = GUM_Animator_update(&pSelf->x, dt);
    const bool changedY = GUM_Animator_update(&pSelf->y, dt);
    return changedX || changedY;
}
