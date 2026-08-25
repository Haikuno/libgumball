#ifndef GUM_COMMON_H
#define GUM_COMMON_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__common_8h.html

/*!  \file
 *   \ingroup elements
 *
 *   gumball_common is a collection of functions and macros that are
 *   common to all elements in libGumball.
 *
 *   \author    2025, 2026 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include <gimbal/gimbal_meta.h>
#include <gimbal/containers/gimbal_ring_list.h>
#include <gumball/types/gumball_renderer.h>

//! Passes a backend event to libGumball.
GBL_EXPORT void GUM_processEvent(const void* pEvent) GBL_NOEXCEPT;

//! Updates all UI elements
#define GUM_update()                   ((GUM_update)())

//! Disables updating for a given element
#define GUM_update_disable(element)    ((GUM_update_disable)    (GBL_OBJECT(element)))

//! Enables updating for a given element
#define GUM_update_enable(element)     ((GUM_update_enable)     (GBL_OBJECT(element)))

//! Disables updating for a given element and all of its children
#define GUM_update_disableAll(element) ((GUM_update_disableAll) (GBL_OBJECT(element)))

//! Enables updating for a given element and all of its children
#define GUM_update_enableAll(element)  ((GUM_update_enableAll)  (GBL_OBJECT(element)))

/*!  \brief Draws all UI elements below the active Root
 *
 *   Elements are ordered by z-index, hierarchy depth, then Root order.
 *   Draw-order changes made while drawing take effect on the next draw.
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

//! Adds the second element as a child of the first and updates its layout.
#define GUM_add_child(self, child)    ((GUM_add_child)    (GBL_OBJECT(self), GBL_OBJECT(child)))

//! Removes the second element as a child of the first and updates its layout.
#define GUM_remove_child(self, child) ((GUM_remove_child) (GBL_OBJECT(self), GBL_OBJECT(child)))

/*!
 *  Takes in a UI element and an index, returns the child of that element at that index as a
 *  GblObject that you can cast to the appropriate type.
 *
 *  Example:
 *  \code GUM_Widget* pChild = GUM_WIDGET(GUM_get_child_at(pObj, 0)); \endcode
*/
#define GUM_get_child_at(self, index) (GblObject_findChildByIndex(GBL_OBJECT(self), index))

//! Increments the reference count of a UI element, returning a pointer to it.
#define GUM_ref(obj)                  ((GUM_ref)(GBL_OBJECT(obj)))

//! Decrements the reference count of an element and its children.
#define GUM_unref(obj)                ((GUM_unref)(GBL_OBJECT(obj)))

/*!  Connects an element's signal to a callback.
 *   The callback should take in a pointer to the element that emits the signal, and return void.
 *   Some signals also include a pointer to the emitting input device. (e.g. onPress, onRelease)
 *   See GUM_Widget for signal names
 *
 *   \code {.c}
 *       void buttonCallback(GUM_Widget* pSelf) {
 *           printf("Button pressed!\n");
 *           printf("UserData: %i\n", *(int*)ud);
 *       }
 *
 *       auto pButton = GUM_Button_create();
 *       GUM_connect(pButton, "onPressConfirm", buttonCallback, &data);
 *   \endcode
 *
 *   \note You can also pass multiple signals at once, with the pattern: "signalName", callbackFn
*/
#define GUM_connect(...) GUM_connect_(__VA_ARGS__)

//! Looks up the property of an element by name, storing its value in the pointer passed as a variadic argument
#define GUM_property(obj, name, /*value*/ ...) (GblObject_property(GBL_OBJECT(obj), name, __VA_ARGS__))

//! Sets the property with the given name. Setting parent also updates libGumball's hierarchy.
#define GUM_setProperty(obj, name, /*value*/...) ((GUM_setProperty)(GBL_OBJECT(obj), name __VA_OPT__(,) __VA_ARGS__))

//! Creates a NULL-terminated list that can be passed to the inherited children property.
#define GUM_childrenList(child1, /*child2, child3, */ ...) \
    (GblRingList_create(child1 __VA_OPT__(,) __VA_ARGS__, nullptr))

////////// Implementation details, Grugs please ignore
//!\cond GRUGLESS
#define GUM_draw_(renderer, ...) (GUM_draw)(renderer)

#define GUM_connect_(emitter, ...) \
    ({ \
        typeof(emitter) _gum_emitter_ = (emitter); \
        GUM_CONNECT_PAIRS_(_gum_emitter_, __VA_ARGS__) \
        _gum_emitter_; \
    })
#define GUM_CONNECT_PAIRS_(emitter, ...) \
    GBL_GLUE(GUM_CONNECT_PAIRS__, GBL_NARG(__VA_ARGS__))(emitter, __VA_ARGS__)
#define GUM_CONNECT_PAIRS__2(emitter, signal, callback) \
    GBL_CONNECT(emitter, signal, callback);
#define GUM_CONNECT_PAIRS__4(emitter, signal, callback, ...) \
    GUM_CONNECT_PAIRS__2(emitter, signal, callback) \
    GUM_CONNECT_PAIRS__2(emitter, __VA_ARGS__)
#define GUM_CONNECT_PAIRS__6(emitter, signal, callback, ...) \
    GUM_CONNECT_PAIRS__2(emitter, signal, callback) \
    GUM_CONNECT_PAIRS__4(emitter, __VA_ARGS__)
#define GUM_CONNECT_PAIRS__8(emitter, signal, callback, ...) \
    GUM_CONNECT_PAIRS__2(emitter, signal, callback) \
    GUM_CONNECT_PAIRS__6(emitter, __VA_ARGS__)
#define GUM_CONNECT_PAIRS__10(emitter, signal, callback, ...) \
    GUM_CONNECT_PAIRS__2(emitter, signal, callback) \
    GUM_CONNECT_PAIRS__8(emitter, __VA_ARGS__)
#define GUM_CONNECT_PAIRS__12(emitter, signal, callback, ...) \
    GUM_CONNECT_PAIRS__2(emitter, signal, callback) \
    GUM_CONNECT_PAIRS__10(emitter, __VA_ARGS__)
#define GUM_CONNECT_PAIRS__14(emitter, signal, callback, ...) \
    GUM_CONNECT_PAIRS__2(emitter, signal, callback) \
    GUM_CONNECT_PAIRS__12(emitter, __VA_ARGS__)
#define GUM_CONNECT_PAIRS__16(emitter, signal, callback, ...) \
    GUM_CONNECT_PAIRS__2(emitter, signal, callback) \
    GUM_CONNECT_PAIRS__14(emitter, __VA_ARGS__)
#define GUM_CONNECT_PAIRS__18(emitter, signal, callback, ...) \
    GUM_CONNECT_PAIRS__2(emitter, signal, callback) \
    GUM_CONNECT_PAIRS__16(emitter, __VA_ARGS__)
#define GUM_CONNECT_PAIRS__20(emitter, signal, callback, ...) \
    GUM_CONNECT_PAIRS__2(emitter, signal, callback) \
    GUM_CONNECT_PAIRS__18(emitter, __VA_ARGS__)

GBL_EXPORT GBL_RESULT (GUM_draw)              (GUM_Renderer* pRenderer)                  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_update)            (void)                                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_update_disable)    (GblObject* pSelf)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_update_enable)     (GblObject* pSelf)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_update_disableAll) (GblObject* pSelf)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_update_enableAll)  (GblObject* pSelf)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_draw_enable)       (GblObject* pSelf)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_draw_disable)      (GblObject* pSelf)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_draw_enableAll)    (GblObject* pSelf)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_draw_disableAll)   (GblObject* pSelf)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_setProperty)       (GblObject* pSelf, const char* pName, ...) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_add_child)         (GblObject* pSelf, GblObject* pChild)     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_remove_child)      (GblObject* pSelf, GblObject* pChild)     GBL_NOEXCEPT;
GBL_EXPORT GblObject* (GUM_ref)               (GblObject* pSelf)                        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT (GUM_unref)             (GblObject* pSelf)                        GBL_NOEXCEPT;
//!\endcond

#endif
