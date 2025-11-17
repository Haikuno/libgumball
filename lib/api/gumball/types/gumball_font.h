#ifndef GUM_FONT_H
#define GUM_FONT_H

/*! \file
 *  \ingroup types
 *
 *  GUM_Font is the font type used in libGumball
 *
 *  \author 	2025 Agustín Bellagamba
 *  \copyright 	MIT License
*/
#include <gimbal/gimbal_meta.h>
#include <gumball/types/gumball_vector2.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_FONT_TYPE (GBL_TYPEID(GUM_Font))					//!< Returns the GUM_Font Type UUID
#define GUM_TEXT_ALIGNMENT_TYPE (GBL_TYPEID(GUM_TextAlignment))	//!< Returns the GUM_TextAlignment Type UUID
//! @}

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Font);

typedef enum GUM_TextAlignment {
	GUM_TEXT_ALIGN_CENTER,
	GUM_TEXT_ALIGN_TOP,
	GUM_TEXT_ALIGN_RIGHT,
	GUM_TEXT_ALIGN_BOTTOM,
	GUM_TEXT_ALIGN_LEFT
} GUM_TextAlignment;

//! \cond
GblType GUM_Font_type(void);
GblType	GUM_TextAlignment_type(void);
//! \endcond

//! 			Returns a new GUM_Font
GUM_Font 	  *GUM_Font_create(void *pFont);

//!				Returns the size of the passed text, given the font and font size
GUM_Vector2    GUM_Font_measureText(GUM_Font *pFont, GblStringRef *pText, uint8_t fontSize);

GBL_DECLS_END

#endif // GUM_FONT_H
