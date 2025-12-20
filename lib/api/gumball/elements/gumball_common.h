#ifndef GUM_COMMON_H
#define GUM_COMMON_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__common_8h.html

/*!  \file
 *   \ingroup elements
 *
 *   gumball_common is a collection of functions and macros that are
 *   common to all elements in libGumball.
 *
 *   \author       2025 Agustín Bellagamba
 *   \copyright    MIT License
*/

#include <gimbal/gimbal_meta.h>
#include <gumball/types/gumball_renderer.h>

//! Updates all the UI elements
#define GUM_update()                   ((GUM_update)())

//! Disables updating for a given element
#define GUM_update_disable(element)    ((GUM_update_disable)    (GBL_OBJECT(element)))

//! Enables updating for a given element
#define GUM_update_enable(element)     ((GUM_update_enable)     (GBL_OBJECT(element)))

//! Disables updating for a given element and all of its children
#define GUM_update_disableAll(element) ((GUM_update_disableAll) (GBL_OBJECT(element)))

//! Enables updating for a given element and all of its children
#define GUM_update_enableAll(element)  ((GUM_update_enableAll)  (GBL_OBJECT(element)))

/*!  \brief Draws all the UI elements in the draw queue
 *
 *   Drawable elements are added to the draw queue when they are created
 *   Elements are drawn from z-index 0 to z-index 255, with 255 being the front
 *   If two elements have the same z-index, they are drawn in the order they were created (or enabled!)
 *   Optionally takes in a GUM_Renderer if your backend needs one (defaults to nullptr)
*/
#define GUM_draw(/* renderer=nullptr */...) GUM_draw_(__VA_OPT__(__VA_ARGS__,) nullptr)


//! Disables drawing for a given element
#define GUM_draw_disable(obj)         ((GUM_draw_disable)    (GBL_OBJECT(obj)))

//! Enables drawing for a given element
#define GUM_draw_enable(obj)          ((GUM_draw_enable)     (GBL_OBJECT(obj)))

//! Disables drawing for a given element and all of its children
#define GUM_draw_disableAll(obj)      ((GUM_draw_disableAll) (GBL_OBJECT(obj)))

//! Enables drawing for a given element and all of its children
#define GUM_draw_enableAll(obj)       ((GUM_draw_enableAll)  (GBL_OBJECT(obj)))

//! Takes in two UI elements, adds the second one as a child of the first.
#define GUM_add_child(self, child)    (GblObject_addChild(GBL_OBJECT(self), GBL_OBJECT(child)))

//! Takes in two UI elements, removes the second one as a child of the first.
#define GUM_remove_child(self, child) (GblObject_removeChild(GBL_OBJECT(self), GBL_OBJECT(child)))

/*!
 *  Takes in a UI element and an index, returns the child of that element at that index as a
 *  GblObject that you can cast to the appropriate type.
 *
 *  Example:
 *  \code GUM_Widget* pChild = GUM_WIDGET(GUM_get_child_at(pObj, 0)); \endcode
*/
#define GUM_get_child_at(self, index) (GblObject_findChildByIndex(GBL_OBJECT(self), index))

//! Decrements the reference count of a UI element, freeing it if it reaches zero.
//! Also recursively unrefs all of its children.
#define GUM_unref(obj)                ((GUM_unref)(GBL_OBJECT(obj)))

/*!  Connects an element's (typically a GUM_Button) signal to a callback.
 *   The callback should take in a pointer to the UI element type that emitted the signal, and return void.
 *   Optionally takes in userdata (a void* of whatever you want to pass in).
 *   See GUM_Button for signal names
 *
 *   \code {.c}
 *       void buttonCallback(GUM_Button* pButton) {
 *           void* ud = GUM_userData();
 *           printf("Button pressed!\n");
 *           printf("UserData: %i\n", *(int*)ud);
 *       }
 *
 *       auto pButton = GUM_Button_create();
 *       int data = 42;
 *       GUM_connect(pButton, "onPressPrimary", buttonCallback, &data);
 *   \endcode
*/
#define GUM_connect(emitter, signal, callback, /* userdata=nullptr */...) (GBL_CONNECT(emitter, signal, emitter, callback __VA_OPT__(,) __VA_ARGS__))

//! Inside a callback connected to a signal, returns the userdata that was passed in
#define GUM_userData() GblClosure_currentUserdata()

//! Looks up the property of an element by name, storing its value in the pointer passed as a variadic argument
#define GUM_property(obj, name, /*value*/ ...) (GblObject_property(GBL_OBJECT(obj), name, __VA_ARGS__))

//! Sets the property with the given name to the value given by the pointer passed through the variadic argument list
#define GUM_setProperty(obj, name, /*value*/...) (GblObject_setProperty(GBL_OBJECT(obj), name, __VA_ARGS__))

////////// Implementation details, Grugs please ignore
//!\cond
#define GUM_draw_(renderer, ...) (GUM_draw)(renderer)

GBL_EXPORT GBL_RESULT    (GUM_draw)             (GUM_Renderer* pRenderer)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    (GUM_update)           (void)                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    (GUM_update_disable)   (GblObject* pSelf)          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    (GUM_update_enable)    (GblObject* pSelf)          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    (GUM_update_disableAll)(GblObject* pSelf)          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    (GUM_update_enableAll) (GblObject* pSelf)          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT    (GUM_unref)            (GblObject* pSelf)          GBL_NOEXCEPT;
GBL_EXPORT void          (GUM_draw_enable)      (GblObject* pSelf)          GBL_NOEXCEPT;
GBL_EXPORT void          (GUM_draw_disable)     (GblObject* pSelf)          GBL_NOEXCEPT;
GBL_EXPORT void          (GUM_draw_enableAll)   (GblObject* pSelf)          GBL_NOEXCEPT;
GBL_EXPORT void          (GUM_draw_disableAll)  (GblObject* pSelf)          GBL_NOEXCEPT;
//!\endcond

#endif
