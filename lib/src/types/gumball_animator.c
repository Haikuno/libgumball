#include <gumball/types/gumball_animator.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_logger.h>
#include <gimbal/meta/instances/gimbal_box.h>
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

// ===== Property Tweening =====

constexpr int GUM_ANIMATE_MAX_ACTIVE_ = 64; // TODO: grow dynamically if this ever isn't enough

typedef struct {
    GblObject*   pObject;
    GblQuark     property;
    GUM_Animator animator;
    bool         inUse;
} GUM_PropertyTween_;

static GUM_PropertyTween_ prop_tweens_[GUM_ANIMATE_MAX_ACTIVE_];

static GUM_PropertyTween_* GUM_Animator_findTween_(GblObject* pObject, GblQuark property) {
    for (size_t i = 0; i < GUM_ANIMATE_MAX_ACTIVE_; ++i) {
        GUM_PropertyTween_* pTween = &prop_tweens_[i];

        if (pTween->inUse && pTween->pObject == pObject && pTween->property == property)
            return pTween;
    }

    return nullptr;
}

static GUM_PropertyTween_* GUM_Animator_allocTween_(void) {
    for (size_t i = 0; i < GUM_ANIMATE_MAX_ACTIVE_; ++i) {
        if (!prop_tweens_[i].inUse)
            return &prop_tweens_[i];
    }

    GUM_LOG_ERROR("Ran out of property tween slots! Bump GUM_ANIMATE_MAX_ACTIVE_.");
    return nullptr;
}

static void GUM_animateStart_(GblObject* pObject, const char* pProperty, float target, float duration,
                              GUM_EasingType easing, GUM_EasingFn pFnEase) {
    const GblQuark      quark  = GblQuark_fromString(pProperty);
    GUM_PropertyTween_* pTween = GUM_Animator_findTween_(pObject, quark);

    if (!pTween) {
        pTween = GUM_Animator_allocTween_();
        if (!pTween) return;

        GBL_VARIANT(value);
        GblObject_propertyVariantByQuark(pObject, quark, &value);
        const float current = GblVariant_toFloat(&value); // converts properties (like alpha) via the registered converter table
        GblVariant_destruct(&value);

        pTween->pObject  = pObject;
        pTween->property = quark;
        pTween->inUse    = true;
        pTween->animator = GUM_Animator_make(current, duration, easing);
    } else {
        pTween->animator.duration = duration;
        pTween->animator.easing   = easing;
    }

    pTween->animator.pFnEase = pFnEase;
    GUM_Animator_set(&pTween->animator, target);
}

void (GUM_animate)(GblObject* pObject, const char* pProperty,
                            float target, float duration, GUM_EasingType easing) {
    GUM_animateStart_(pObject, pProperty, target, duration, easing, nullptr);
}

void (GUM_animateCustom)(GblObject* pObject, const char* pProperty,
                                  float target, float duration, GUM_EasingFn pFnEase) {
    GUM_animateStart_(pObject, pProperty, target, duration, GUM_EASE_CUSTOM, pFnEase);
}

static void GUM_Animator_freeTween_(GUM_PropertyTween_* pTween) {
    GUM_Animator_setOnDone(&pTween->animator, nullptr); // releases any attached closure's ref
    pTween->inUse = false;
}

static GBL_RESULT GUM_animateMarshal_(GblClosure* pClosure, GblVariant* pRetValue,
                                      size_t argCount, GblVariant* pArgs, GblPtr pMarshalData) {
    GBL_UNUSED(pRetValue, argCount, pArgs, pMarshalData);

    const GUM_DoneFn pFnDone = (GUM_DoneFn)GblCClosure_callback(GBL_C_CLOSURE(pClosure));
    pFnDone(GUM_WIDGET(GblBox_userdata(GBL_BOX(pClosure))));

    return GBL_RESULT_SUCCESS;
}


void (GUM_animateOnDone)(GblObject* pObject, const char* pProperty, GUM_DoneFn pFnDone) {
    GUM_PropertyTween_* pTween = GUM_Animator_findTween_(pObject, GblQuark_fromString(pProperty));
    if (!pTween) return;

    if (!pFnDone) {
        GUM_Animator_setOnDone(&pTween->animator, nullptr); // clears any existing callback
        return;
    }

    GblClosure* pClosure = GBL_CLOSURE(GblCClosure_create((GblFnPtr)pFnDone, pObject));
    GblClosure_setMarshal(pClosure, GUM_animateMarshal_);
    GUM_Animator_setOnDone(&pTween->animator, pClosure);
    GblClosure_unref(pClosure); // the tween holds its own ref now
}

void (GUM_animateCancel)(GblObject* pObject, const char* pProperty) {
    GUM_PropertyTween_* pTween = GUM_Animator_findTween_(pObject, GblQuark_fromString(pProperty));

    if (pTween)
        GUM_Animator_freeTween_(pTween);
}

void GUM_animate_update_(void) {
    const float dt = GUM_Backend_frametime();

    for (size_t i = 0; i < GUM_ANIMATE_MAX_ACTIVE_; i++) {
        GUM_PropertyTween_* pTween = &prop_tweens_[i];

        if (!pTween->inUse)
            continue;

        if (GUM_Animator_update(&pTween->animator, dt)) {
            GBL_VARIANT(value);
            GblVariant_setFloat(&value, pTween->animator.current);
            GblObject_setPropertyVariantByQuark(pTween->pObject, pTween->property, &value);
            GblVariant_destruct(&value);
        }
    }

    // Second loop to run closure.
    // This prevents use-after-free if the user unrefs the widget on a callback, but still has animations to run on the same frame (that last the same amount of time)
    for (size_t i = 0; i < GUM_ANIMATE_MAX_ACTIVE_; i++) {
        GUM_PropertyTween_* pTween = &prop_tweens_[i];

        if (!pTween->inUse)
            continue;

        if (GUM_Animator_settled(&pTween->animator)) {
            if (pTween->animator.pOnDone)
                GblClosure_invoke(pTween->animator.pOnDone, nullptr, 0, nullptr);
            // Check again, in case the callback fired a new animation.
            if (GUM_Animator_settled(&pTween->animator))
                GUM_Animator_freeTween_(pTween);
        }
    }
}
