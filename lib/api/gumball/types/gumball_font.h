#ifndef GUM_FONT_H
#define GUM_FONT_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__font_8h.html

/*!  \file
 *   \ref        GUM_Font "GUM_Font data structure and hierarchy graph"
 *   \ingroup    types
 *
 *   GUM_Font is the backend-agnostic font type used in libGumball
 *
 *   \todo
 *       - Replace text alignments with flags for TOP, RIGHT, BOTTOM, LEFT, CENTER
 *
 *   \author     2025 Agustín Bellagamba
 *   \copyright  MIT License
*/
#include <gimbal/gimbal_meta.h>
#include <gumball/types/gumball_vector2.h>
#include <gumball/ifaces/gumball_iresource.h>

/*!  \name  Type System
 *   \brief Type UUID and cast operators
 *   @{
*/
#define GUM_FONT_TYPE           (GBL_TYPEID      (GUM_Font))         //!< Returns the GUM_Font Type UUID
#define GUM_FONT(self)          (GBL_CAST        (GUM_Font, self))   //!< Casts an instance of a compatible resource to a GUM_Font
#define GUM_FONT_CLASS(klass)   (GBL_CLASS_CAST  (GUM_Font, klass))  //!< Casts a  class    of a compatible resource to a GUM_FontClass
#define GUM_FONT_CLASSOF(self)  (GBL_CLASSOF     (GUM_Font, self))   //!< Casts an instance of a compatible resource to a GUM_FontClass

#define GUM_TEXT_ALIGNMENT_TYPE (GBL_TYPEID(GUM_TextAlignment))      //!< Returns the GUM_TextAlignment Type UUID
//! @}

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Font);

/*!  \struct     GUM_FontClass
 *   \extends    GblBoxClass
 *   \implements GUM_IResource
 *   \brief      GUM_Font structure
 *
 *   GUM_FontClass derives from GblBoxClass, adding nothing new.
 *   It also implements the GUM_IResource interface
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Font, GblBox, GUM_IResource)

/*!  \struct     GUM_Font
 *   \extends    GblBox
 *   \brief      Backend agnostic font type
*/
GBL_INSTANCE_DERIVE_EMPTY(GUM_Font, GblBox)

typedef enum : uint8_t {
    GUM_TEXT_ALIGN_CENTER,  //!< Align the text to the center of the widget
    GUM_TEXT_ALIGN_TOP,     //!< Align the text to the top of the widget
    GUM_TEXT_ALIGN_RIGHT,   //!< Align the text to the right of the widget
    GUM_TEXT_ALIGN_BOTTOM,  //!< Align the text to the bottom of the widget
    GUM_TEXT_ALIGN_LEFT     //!< Align the text to the left of the widget
} GUM_TextAlignment;

//! \cond
GblType GUM_Font_type(void);
GblType GUM_TextAlignment_type(void);
//! \endcond

//! Returns the size of the passed text, given the font and font size
GBL_EXPORT GUM_Vector2 GUM_Font_measureText(GUM_Font* pFont, GblStringRef* pText, uint8_t fontSize) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GUM_FONT_H
