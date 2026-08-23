#ifndef GUM_WIDGET_H
#define GUM_WIDGET_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__widget_8h.html

/*!  \file
 *   \ref     GUM_Widget "GUM_Widget data structure and hierarchy graph"
 *   \ingroup elements
 *
 *   GUM_Widget is the most basic, fundamental element.
 *
 *   It is the parent class for all drawable elements in libGumball,
 *   holding variables that are common to all drawable elements, such as position, size, and color.
 *
 *   \todo
 *       - Separate isRelative into position and size
 *       - Add GUM_Vector2 position and size as properties
 *       - Make border highlight configurable (color, thickness)
 *       - Document signals
 *
 *   \author    2025, 2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gimbal/gimbal_meta.h>
#include <gimbal/gimbal_strings.h>
#include <gimbal/gimbal_core.h>

#include <gumball/types/gumball_font.h>
#include <gumball/types/gumball_texture.h>
#include <gumball/types/gumball_renderer.h>
#include <gumball/types/gumball_rectangle.h>
#include <gumball/types/gumball_animator.h>
#include <gumball/gumball_events.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_WIDGET_TYPE             (GBL_TYPEID     (GUM_Widget))           //!< Returns the GUM_Widget Type UUID
#define GUM_WIDGET(self)            (GBL_CAST       (GUM_Widget, self))     //!< Casts an instance of a compatible element to a GUM_Widget
#define GUM_WIDGET_CLASS(klass)     (GBL_CLASS_CAST (GUM_Widget, klass))    //!< Casts a class of a compatible element to a GUM_WidgetClass
#define GUM_WIDGET_CLASSOF(self)    (GBL_CLASSOF    (GUM_Widget, self))     //!< Casts an instance of a compatible element to a GUM_WidgetClass
//! @}

#define GBL_SELF_TYPE                GUM_Widget

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Widget);

/*!
 *    \struct  GUM_WidgetClass
 *    \extends GblObjectClass
 *    \brief   GUM_Widget structure
 *
 *    Adds virtual functions for activation, deactivation, updating, drawing, and input events.
*/
//! \cond
GBL_CLASS_DERIVE(GUM_Widget, GblObject)
    GBL_RESULT (*pFnActivate)    (GBL_SELF);
    GBL_RESULT (*pFnDeactivate)  (GBL_SELF);
    GBL_RESULT (*pFnUpdate)      (GBL_SELF);
    GBL_RESULT (*pFnDraw)        (GBL_SELF, GUM_Renderer*    pRenderer);
    GBL_RESULT (*pFnInputEvent)  (GBL_SELF, GUM_Event_Input* pEvent);
GBL_CLASS_END
//! \endcond

/*!
 *    \class   GUM_Widget
 *    \extends GblObject
 *    \brief   Basic widget element
*/

/*!
 *    \name  Properties
 *    \brief Widget properties you can set/get at or after creation.
 *    @{
*/
//! \cond
GBL_INSTANCE_DERIVE(GUM_Widget, GblObject)
//! \endcond
    float              x;                        //!< Horizontal position of the widget.                                             Default value is 0
    float              y;                        //!< Vertical position of the widget.                                               Default value is 0
    float              w;                        //!< Width of the widget.                                                           Default value is 200
    float              h;                        //!< Height of the widget.                                                          Default value is 200
    uint8_t            r;                        //!< Red component of the widget color.                                             Default value is 0
    uint8_t            g;                        //!< Green component of the widget color.                                           Default value is 255
    uint8_t            b;                        //!< Blue component of the widget color.                                            Default value is 0
    uint8_t            a;                        //!< Alpha component of the widget color.                                           Default value is 255
    uint8_t            border_r;                 //!< Red component of the border color.                                             Default value is 0
    uint8_t            border_g;                 //!< Green component of the border color.                                           Default value is 0
    uint8_t            border_b;                 //!< Blue component of the border color.                                            Default value is 0
    uint8_t            border_a;                 //!< Alpha component of the border color.                                           Default value is 0
    uint8_t            border_width;             //!< Width of the border, in pixels.                                                Default value is 4
    float              border_radius;            //!< Radius of the border. Property writes clamp to 0.0f..1.0f.                     Default value is 0
    bool               border_highlight;         //!< If the border should be highlighted.                                           Default value is false
    bool               isRelative;               //!< If the widget's position and size should be relative to its parent.            Default value is false
    bool               isSelectable;             //!< If the widget is visible to the navigation system.                             Default value is false
    bool               isSelectedByDefault;      //!< If the widget should be selected by default when no other is focused.          Default value is false
    bool               isInteractive;            //!< If the widget is eligible for input events.                                    Default value is true
    bool               shouldUpdate;             //!< If the widget should be updated.                                               Default value is true
    GUM_TextAlignment  textAlignment;            //!< Alignment of the widget's label.                                               Default value is GUM_TEXT_ALIGN_CENTER
    uint8_t            font_size;                //!< Font size of the widget's label.                                               Default value is 22
    uint8_t            font_r;                   //!< Red component of the font color.                                               Default value is 255
    uint8_t            font_g;                   //!< Green component of the font color.                                             Default value is 255
    uint8_t            font_b;                   //!< Blue component of the font color.                                              Default value is 255
    uint8_t            font_a;                   //!< Alpha component of the font color.                                             Default value is 255
    uint8_t            font_border_r;            //!< Red component of the font border color.                                        Default value is 0
    uint8_t            font_border_g;            //!< Green component of the font border color.                                      Default value is 0
    uint8_t            font_border_b;            //!< Blue component of the font border color.                                       Default value is 0
    uint8_t            font_border_a;            //!< Alpha component of the font border color.                                      Default value is 0
    uint8_t            font_border_thickness;    //!< Width of the font border, in pixels.                                           Default value is 1
GBL_INSTANCE_END
//! @}

GBL_PROPERTIES(GUM_Widget,
    (x,                     GBL_GENERIC, (READ, WRITE),          GBL_FLOAT_TYPE         ),
    (y,                     GBL_GENERIC, (READ, WRITE),          GBL_FLOAT_TYPE         ),
    (w,                     GBL_GENERIC, (READ, WRITE),          GBL_FLOAT_TYPE         ),
    (h,                     GBL_GENERIC, (READ, WRITE),          GBL_FLOAT_TYPE         ),
    (isRelative,            GBL_GENERIC, (READ, WRITE),          GBL_BOOL_TYPE          ),
    (isInteractive,         GBL_GENERIC, (READ, WRITE),          GBL_BOOL_TYPE          ),
    (isActive,              GBL_GENERIC, (READ, WRITE),          GBL_BOOL_TYPE          ),
    (isSelectable,          GBL_GENERIC, (READ, WRITE),          GBL_BOOL_TYPE          ),
    (isSelectedByDefault,   GBL_GENERIC, (READ, WRITE),          GBL_BOOL_TYPE          ),
    (color,                 GBL_GENERIC, (READ, WRITE),          GBL_UINT32_TYPE        ),
    (border_color,          GBL_GENERIC, (READ, WRITE),          GBL_UINT32_TYPE        ),
    (font_color,            GBL_GENERIC, (READ, WRITE),          GBL_UINT32_TYPE        ),
    (font_border_color,     GBL_GENERIC, (READ, WRITE),          GBL_UINT32_TYPE        ),
    (r,                     GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (g,                     GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (b,                     GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (a,                     GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (border_r,              GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (border_g,              GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (border_b,              GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (border_a,              GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (border_width,          GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (border_radius,         GBL_GENERIC, (READ, WRITE),          GBL_FLOAT_TYPE         ),
    (border_highlight,      GBL_GENERIC, (READ, WRITE),          GBL_BOOL_TYPE          ),
    (label,                 GBL_GENERIC, (READ, WRITE),          GBL_STRING_TYPE        ),
    (labelAcquire,          GBL_GENERIC, (      WRITE, ACQUIRE), GBL_STRING_TYPE        ), // Write-only, takes ownership of the given GblStringRef* and stores it as its label.
    (texture,               GBL_GENERIC, (READ, WRITE),          GUM_TEXTURE_TYPE       ),
    (textAlignment,         GBL_GENERIC, (READ, WRITE),          GUM_TEXT_ALIGNMENT_TYPE),
    (font,                  GBL_GENERIC, (READ, WRITE),          GUM_FONT_TYPE          ),
    (font_size,             GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (font_r,                GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (font_g,                GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (font_b,                GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (font_a,                GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (font_border_r,         GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (font_border_g,         GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (font_border_b,         GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (font_border_a,         GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (font_border_thickness, GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         ),
    (z_index,               GBL_GENERIC, (READ, WRITE),          GBL_UINT8_TYPE         )
)

GBL_SIGNALS(GUM_Widget,
    (onPress,             (GBL_INSTANCE_TYPE, pReceiver), (GUM_EVENT_INPUT_TYPE, pEvent)),
    (onRelease,           (GBL_INSTANCE_TYPE, pReceiver), (GUM_EVENT_INPUT_TYPE, pEvent)),
    (onPressConfirm,      (GBL_INSTANCE_TYPE, pReceiver)),
    (onPressCancel,       (GBL_INSTANCE_TYPE, pReceiver)),
    (onPressMoveUp,       (GBL_INSTANCE_TYPE, pReceiver)),
    (onPressMoveDown,     (GBL_INSTANCE_TYPE, pReceiver)),
    (onPressMoveLeft,     (GBL_INSTANCE_TYPE, pReceiver)),
    (onPressMoveRight,    (GBL_INSTANCE_TYPE, pReceiver)),
    (onPressUnbound,      (GBL_INSTANCE_TYPE, pReceiver)),
    (onReleaseConfirm,    (GBL_INSTANCE_TYPE, pReceiver)),
    (onReleaseCancel,     (GBL_INSTANCE_TYPE, pReceiver)),
    (onReleaseMoveUp,     (GBL_INSTANCE_TYPE, pReceiver)),
    (onReleaseMoveDown,   (GBL_INSTANCE_TYPE, pReceiver)),
    (onReleaseMoveLeft,   (GBL_INSTANCE_TYPE, pReceiver)),
    (onReleaseMoveRight,  (GBL_INSTANCE_TYPE, pReceiver)),
    (onReleaseUnbound,    (GBL_INSTANCE_TYPE, pReceiver)),
    (onActivate,          (GBL_INSTANCE_TYPE, pReceiver)),
    (onDeactivate,        (GBL_INSTANCE_TYPE, pReceiver)),
    (onFocusGained,       (GBL_INSTANCE_TYPE, pReceiver), (GUM_INPUTDEVICE_TYPE, pDevice)), //!< Emitted when a device navigates focus onto this widget
    (onFocusLost,         (GBL_INSTANCE_TYPE, pReceiver), (GUM_INPUTDEVICE_TYPE, pDevice))  //!< Emitted when a device navigates focus away from this widget
)

GblType GUM_Widget_type(void) GBL_NOEXCEPT;

/*! \name Runtime state
 *  @{ */
GBL_EXPORT GblBool       GUM_Widget_isActive  (const GUM_Widget* pSelf) GBL_NOEXCEPT; //!< Current active state.
GBL_EXPORT GblBool       GUM_Widget_isFocused (const GUM_Widget* pSelf) GBL_NOEXCEPT; //!< True if focused by any input device.
GBL_EXPORT uint8_t       GUM_Widget_zIndex    (const GUM_Widget* pSelf) GBL_NOEXCEPT; //!< Current z-index.
GBL_EXPORT GblStringRef* GUM_Widget_label     (const GUM_Widget* pSelf) GBL_NOEXCEPT; //!< Borrowed label.
GBL_EXPORT GUM_Font*     GUM_Widget_font      (const GUM_Widget* pSelf) GBL_NOEXCEPT; //!< Borrowed font.
GBL_EXPORT GUM_Texture*  GUM_Widget_texture   (const GUM_Widget* pSelf) GBL_NOEXCEPT; //!< Borrowed texture.
//! @}

//! Returns a new GUM_Widget. Optionally takes in a list of Name/Value pairs for properties
#define GUM_Widget_create(/* property_name, property_value */...) GBL_NEW(GUM_Widget __VA_OPT__(,) __VA_ARGS__)

//! Callback fired when a property tween settles. Receives the widget that was being animated
typedef void (*GUM_Widget_doneFn)(GUM_Widget* pSelf);

/*!  \name  Animation
 *   \brief Eases readable/writable Widget properties that can be converted to/from float.
 *   @{
*/
//! Eases pProperty toward target, using a built-in curve
GBL_EXPORT void GUM_Widget_animate      (GUM_Widget* pSelf, const char* pProperty, float target, float duration, GUM_EasingType easing) GBL_NOEXCEPT;
//! Eases pProperty toward target, using a custom curve
GBL_EXPORT void GUM_Widget_animateCustom(GUM_Widget* pSelf, const char* pProperty, float target, float duration, GUM_EasingFn pFnEase)  GBL_NOEXCEPT;
//! Fires pFnDone when pProperty's current tween settles.
GBL_EXPORT void GUM_Widget_animateOnDone(GUM_Widget* pSelf, const char* pProperty, GUM_Widget_doneFn pFnDone)                           GBL_NOEXCEPT;
//! Cancels pProperty's active tween, freezing it at its current value
GBL_EXPORT void GUM_Widget_animateCancel(GUM_Widget* pSelf, const char* pProperty) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_WIDGET_H
