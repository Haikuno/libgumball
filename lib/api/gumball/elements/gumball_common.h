#ifndef GUM_COMMON_H
#define GUM_COMMON_H

#include <gimbal/gimbal_meta.h>
#include <gumball/types/gumball_renderer.h>

// Takes in a UI element, updates it and all of its children.
#define GUM_update(obj) 			 ((GUM_update)(GBL_OBJECT(obj)))

// Takes in two UI elements, adds the second one as a child of the first.
#define GUM_add_child(self, child) 	 (GblObject_addChild(GBL_OBJECT(self), GBL_OBJECT(child)))

// Takes in two UI elements, removes the second one as a child of the first.
#define GUM_remove_child(self, child) (GblObject_removeChild(GBL_OBJECT(self), GBL_OBJECT(child)))

// Takes in a UI element and an index, returns the child of that element at that index as a
// GblObject that you can cast to the appropriate type.
// Example : GUM_Widget *pChild = GUM_WIDGET(GUM_get_child_at(pObj, 0));
#define GUM_get_child_at(self, index) (GblObject_findChildByIndex(GBL_OBJECT(self), index))

// Decrements the reference count of a UI element, freeing it if it reaches zero.
// Also recursively unrefs all of its children.
#define GUM_unref(obj)				  ((GUM_unref)(GBL_OBJECT(obj)))

/*
	Connects a UI element's (typically a GUM_Button) signal to a callback. Optionally takes in userdata (a void* of whatever you want to pass in)
	The callback should take in a pointer to the UI element type that emitted the signal, and return void.
	Use GUM_userData() to get the userdata in your callback
	See GUM_button.h for signal names
*/
#define GUM_connect(emitter, signal, callback, ...) (GBL_CONNECT(emitter, signal, emitter, callback __VA_OPT__(,) __VA_ARGS__))

// Inside a callback connected to a signal, returns the userdata that was passed in
#define GUM_userData (GblClosure_currentUserdata())

/*
	Draws all the UI elements in the draw queue
	Drawable elements are added to the draw queue when they are created
	Elements are drawn from z-index 0 to z-index 255, with 255 being the front
	If two elements have the same z-index, they are drawn in the order they were created (or enabled!)
*/
GBL_RESULT GUM_draw(GUM_Renderer *pRenderer);

// Disables drawing for a given widget
#define GUM_draw_disable(obj) 	((GUM_draw_disable)		(GBL_OBJECT(obj)))

// Enables drawing for a given widget
#define GUM_draw_enable(obj) 	((GUM_draw_enable)		(GBL_OBJECT(obj)))

// Disables drawing for a given widget and all of its children
#define GUM_draw_disableAll(obj) ((GUM_draw_disableAll)	(GBL_OBJECT(obj)))

// Enables drawing for a given widget and all of its children
#define GUM_draw_enableAll(obj) 	((GUM_draw_enableAll)	(GBL_OBJECT(obj)))

// Functions used for macros. Ignore.
GBL_RESULT	(GUM_update)			(GblObject *pSelf);
GBL_RESULT	(GUM_unref)				(GblObject *pSelf);
void		(GUM_draw_enable)		(GblObject *pSelf);
void		(GUM_draw_disable)		(GblObject *pSelf);
void		(GUM_draw_enableAll)	(GblObject *pSelf);
void		(GUM_draw_disableAll)	(GblObject *pSelf);

#endif