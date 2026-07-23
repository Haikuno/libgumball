#ifndef GUM_ANIMATOR_H
#define GUM_ANIMATOR_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__animator_8h.html

/*!  \file
 *   \ingroup types
 *
 *   GUM_Animator is a simple duration-based tween.
 *   It eases a float value from one value to another over a fixed
 *   amount of time, shaping its progress with either a built-in
 *   easing curve or a custom one of your own.
 *
 *   \author     2026 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include <gimbal/meta/signals/gimbal_closure.h>
#include <gimbal/meta/instances/gimbal_object.h>
#include <gumball/elements/gumball_widget.h>

GBL_DECLS_BEGIN

/*!  \enum   GUM_EasingType
 *   \brief  Built-in easing curves for a GUM_Animator to ease with
*/
GBL_ENUM(GUM_EasingType,
    (GUM_EASE_LINEAR,       "nick", 0),  //!< No easing, constant speed
    (GUM_EASE_QUAD_IN,      "nick", 1),  //!< Accelerating from zero
    (GUM_EASE_QUAD_OUT,     "nick", 2),  //!< Decelerating to zero
    (GUM_EASE_QUAD_IN_OUT,  "nick", 3),  //!< Accelerating, then decelerating
    (GUM_EASE_CUBIC_IN,     "nick", 4),  //!< Accelerating from zero, more pronounced
    (GUM_EASE_CUBIC_OUT,    "nick", 5),  //!< Decelerating to zero, more pronounced
    (GUM_EASE_CUBIC_IN_OUT, "nick", 6),  //!< Accelerating, then decelerating, more pronounced
    (GUM_EASE_SINE_IN_OUT,  "nick", 7),  //!< Gentle acceleration and deceleration
    (GUM_EASE_BOUNCE_OUT,   "nick", 8),  //!< Decelerating, with a bounce at the end
    (GUM_EASE_ELASTIC_OUT,  "nick", 9),  //!< Decelerating, with an elastic overshoot at the end
    (GUM_EASE_CUSTOM,       "nick", 10), //!< Uses the animator's pFnEase instead of a built-in curve
    (GUM_EASE_COUNT,        "nick", 11)  //!< Number of entries in this enum
)

//! Maps a normalized progress value in [0, 1] to an eased progress value
typedef float (*GUM_EasingFn)(float t);
typedef void  (*GUM_DoneFn)  (GUM_Widget* pSelf);

/*!  \struct GUM_Animator
 *   \brief  GUM_Animator structure
*/
typedef struct {
    float          from;     //!< Value eased from
    float          to;       //!< Value eased toward
    float          current;  //!< Current, eased value
    float          duration; //!< Total time the ease takes, in seconds
    float          elapsed;  //!< Time elapsed since the last GUM_Animator_set()
    GUM_EasingType easing;   //!< Built-in curve used to shape progress over time
    GUM_EasingFn   pFnEase;  //!< Custom curve, only used when easing == GUM_EASE_CUSTOM
    GblClosure*    pOnDone;  //!< Optional closure invoked once settled. May be nullptr
} GUM_Animator;

#define GUM_ANIMATOR_TYPE (GBL_TYPEID(GUM_Animator)) //!< Returns the GUM_Animator Type UUID

//! Eases pProperty on pObject toward target, using a built-in curve
#define GUM_animate(widget, propertyName, targetValue, duration, easing) \
        (GUM_animate(GBL_OBJECT(widget), propertyName, targetValue, duration, easing))

//! Eases pProperty on pObject toward target, using a custom curve
#define GUM_animateCustom(widget, propertyName, targetValue, duration, easingFunction) \
        (GUM_animateCustom(GBL_OBJECT(widget), propertyName, targetValue, duration, easingFunction))

//! Attaches a closure to fire when the property's tween settles.
#define GUM_animateOnDone(widget, propertyName, doneFn) \
        (GUM_animateOnDone(GBL_OBJECT(widget), propertyName, (GUM_DoneFn)doneFn))

//! Cancels the passed property's tween, freezing it at its current value
#define GUM_animateCancel(widget, propertyName) \
        (GUM_animateCancel)(GBL_OBJECT(widget), propertyName)

//! \cond
GblType GUM_Animator_type(void) GBL_NOEXCEPT;
//! \endcond

/*!  \name  Lifetime & Playback
 *   \brief Methods for creating, retargeting, and advancing an animator
 *   @{
*/
GBL_EXPORT GUM_Animator GUM_Animator_make      (float value, float duration, GUM_EasingType easing) GBL_NOEXCEPT; //!< Returns a new animator sitting at rest on value, using a built-in curve
GBL_EXPORT GUM_Animator GUM_Animator_makeCustom(float value, float duration, GUM_EasingFn pFnEase)  GBL_NOEXCEPT; //!< Returns a new animator sitting at rest on value, using a custom curve
GBL_EXPORT void         GUM_Animator_set       (GUM_Animator* pSelf, float target)                  GBL_NOEXCEPT; //!< Retargets the animator, easing from its current value
GBL_EXPORT bool         GUM_Animator_update    (GUM_Animator* pSelf, float dt)                      GBL_NOEXCEPT; //!< Advances current toward target. Returns true if it moved
GBL_EXPORT bool         GUM_Animator_settled   (const GUM_Animator* pSelf)                          GBL_NOEXCEPT; //!< Returns true if current has reached target
GBL_EXPORT void         GUM_Animator_setOnDone (GUM_Animator* pSelf, GblClosure* pClosure)          GBL_NOEXCEPT; //!< Sets (or replaces) the closure invoked when the animator settles
//! @}

/*!  \name  Built-in Easing Curves
 *   \brief Stateless curve functions
 *   @{
*/
GBL_EXPORT float GUM_Ease_linear      (float t) GBL_NOEXCEPT;
GBL_EXPORT float GUM_Ease_quadIn      (float t) GBL_NOEXCEPT;
GBL_EXPORT float GUM_Ease_quadOut     (float t) GBL_NOEXCEPT;
GBL_EXPORT float GUM_Ease_quadInOut   (float t) GBL_NOEXCEPT;
GBL_EXPORT float GUM_Ease_cubicIn     (float t) GBL_NOEXCEPT;
GBL_EXPORT float GUM_Ease_cubicOut    (float t) GBL_NOEXCEPT;
GBL_EXPORT float GUM_Ease_cubicInOut  (float t) GBL_NOEXCEPT;
GBL_EXPORT float GUM_Ease_sineInOut   (float t) GBL_NOEXCEPT;
GBL_EXPORT float GUM_Ease_bounceOut   (float t) GBL_NOEXCEPT;
GBL_EXPORT float GUM_Ease_elasticOut  (float t) GBL_NOEXCEPT;
//! @}

/*!  \name  Property Tweening
 *   \brief One-call API for animating any readable/writable GBL_GENERIC property
 *          on any GblObject (widget position, color channels, border radius, etc.),
 *          without having to declare or update a GUM_Animator yourself.
 *   @{
*/

//! @}

//! \cond GRUGLESS
//! \note Implementation. You should use the macros instead.
GBL_EXPORT void (GUM_animate)      (GblObject* pObject, const char* pProperty, float target, float duration, GUM_EasingType easing) GBL_NOEXCEPT;
GBL_EXPORT void (GUM_animateCustom)(GblObject* pObject, const char* pProperty, float target, float duration, GUM_EasingFn pFnEase)  GBL_NOEXCEPT;
GBL_EXPORT void (GUM_animateOnDone)(GblObject* pObject, const char* pProperty, GUM_DoneFn pDoneFn)                                  GBL_NOEXCEPT;
GBL_EXPORT void (GUM_animateCancel)(GblObject* pObject, const char* pProperty)                                                      GBL_NOEXCEPT;
void GUM_animate_update_(void)                                                                                                      GBL_NOEXCEPT; //!< Advances every active property tween by one frame. Called by GUM_update()
//! \endcond

GBL_DECLS_END

#endif // GUM_ANIMATOR_H
