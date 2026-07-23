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

#include <gimbal/gimbal_meta.h>
#include <gimbal/meta/signals/gimbal_closure.h>

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
GBL_EXPORT bool         GUM_Animator_update    (GUM_Animator* pSelf, float dt)                      GBL_NOEXCEPT; //!< Advances current toward target. Returns true if it moved. Does NOT invoke pOnDone -- check GUM_Animator_settled() and invoke it yourself after you're done using pSelf
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

GBL_DECLS_END

#endif // GUM_ANIMATOR_H
