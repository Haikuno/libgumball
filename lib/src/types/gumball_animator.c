#include <gumball/types/gumball_animator.h>
#include <math.h>

static const GUM_EasingFn s_easingFns_[GUM_EASE_COUNT] = {
    [GUM_EASE_LINEAR]       = GUM_Ease_linear,
    [GUM_EASE_QUAD_IN]      = GUM_Ease_quadIn,
    [GUM_EASE_QUAD_OUT]     = GUM_Ease_quadOut,
    [GUM_EASE_QUAD_IN_OUT]  = GUM_Ease_quadInOut,
    [GUM_EASE_CUBIC_IN]     = GUM_Ease_cubicIn,
    [GUM_EASE_CUBIC_OUT]    = GUM_Ease_cubicOut,
    [GUM_EASE_CUBIC_IN_OUT] = GUM_Ease_cubicInOut,
    [GUM_EASE_SINE_IN_OUT]  = GUM_Ease_sineInOut,
    [GUM_EASE_BOUNCE_OUT]   = GUM_Ease_bounceOut,
    [GUM_EASE_ELASTIC_OUT]  = GUM_Ease_elasticOut,
    [GUM_EASE_CUSTOM]       = nullptr // resolved to pSelf->pFnEase instead
};

GblType GUM_Animator_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblPointer_register("GUM_Animator");
    }

    return type;
}

GBL_EXPORT GUM_Animator GUM_Animator_make(float value, float duration, GUM_EasingType easing) {
    return (GUM_Animator){ .from     = value,
                           .to       = value,
                           .current  = value,
                           .duration = duration,
                           .easing   = easing};
}

GBL_EXPORT GUM_Animator GUM_Animator_makeCustom(float value, float duration, GUM_EasingFn pFnEase) {
    GUM_Animator animator = GUM_Animator_make(value, duration, GUM_EASE_CUSTOM);
    animator.pFnEase = pFnEase;
    return animator;
}

GBL_EXPORT void GUM_Animator_set(GUM_Animator* pSelf, float target) {
    pSelf->from    = pSelf->current;
    pSelf->to      = target;
    pSelf->elapsed = 0.0f;
}

GBL_EXPORT bool GUM_Animator_update(GUM_Animator* pSelf, float dt) {
    if (GUM_Animator_settled(pSelf))
        return false;

    pSelf->elapsed = GBL_MIN(pSelf->elapsed + dt, pSelf->duration);

    const float        t       = pSelf->duration > 0.0f ? pSelf->elapsed / pSelf->duration : 1.0f;
    const GUM_EasingFn pFnEase = pSelf->easing == GUM_EASE_CUSTOM ? pSelf->pFnEase
                                                                  : s_easingFns_[pSelf->easing];

    pSelf->current = pSelf->from + (pSelf->to - pSelf->from) * pFnEase(t);

    return true;
}

GBL_EXPORT bool GUM_Animator_settled(const GUM_Animator* pSelf) {
    return pSelf->elapsed >= pSelf->duration;
}

GBL_EXPORT void GUM_Animator_setOnDone(GUM_Animator* pSelf, GblClosure* pClosure) {
    if (pSelf->pOnDone)
        GblClosure_unref(pSelf->pOnDone);

    pSelf->pOnDone = pClosure ? GblClosure_ref(pClosure) : nullptr;
}

// NOTE: these functions use Robert Penner's constants, so if you don't like them blame him not me :mink:

GBL_EXPORT float GUM_Ease_linear(float t) {
    return t;
}

GBL_EXPORT float GUM_Ease_quadIn(float t) {
    return t * t;
}

GBL_EXPORT float GUM_Ease_quadOut(float t) {
    return t * (2.0f - t);
}

GBL_EXPORT float GUM_Ease_quadInOut(float t) {
    return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
}

GBL_EXPORT float GUM_Ease_cubicIn(float t) {
    return t * t * t;
}

GBL_EXPORT float GUM_Ease_cubicOut(float t) {
    const float f = t - 1.0f;
    return f * f * f + 1.0f;
}

GBL_EXPORT float GUM_Ease_cubicInOut(float t) {
    if (t < 0.5f) return 4.0f * t * t * t;

    const float f = 2.0f * t - 2.0f;
    return 0.5f * f * f * f + 1.0f;
}

GBL_EXPORT float GUM_Ease_sineInOut(float t) {
    return -(cosf(GBL_F_PI * t) - 1.0f) / 2.0f;
}

GBL_EXPORT float GUM_Ease_bounceOut(float t) {
    if (t < 1.0f / 2.75f) {
        return 7.5625f * t * t;
    } else if (t < 2.0f / 2.75f) {
        t -= 1.5f / 2.75f;
        return 7.5625f * t * t + 0.75f;
    } else if (t < 2.5f / 2.75f) {
        t -= 2.25f / 2.75f;
        return 7.5625f * t * t + 0.9375f;
    } else {
        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + 0.984375f;
    }
}

GBL_EXPORT float GUM_Ease_elasticOut(float t) {
    if (t == 0.0f || t == 1.0f) return t;

    const float p = 0.3f;
    return powf(2.0f, -10.0f * t) * sinf((t - p / 4.0f) * (2.0f * GBL_F_PI) / p) + 1.0f;
}

