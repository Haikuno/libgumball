#ifndef GUM_COLOR_H
#define GUM_COLOR_H

/*! \file
 *  \ingroup types
 *
 *  GUM_Color is a simple RGBA color structure
 *
 *  \author 	2025 Agustín Bellagamba
 *  \copyright 	MIT License
*/

#include <gimbal/gimbal_meta.h>

GBL_DECLS_BEGIN

/*! \struct GUM_Color
 *  \brief  GUM_Color structure
*/
typedef struct {
	uint8_t r; //!< Red component
	uint8_t g; //!< Green component
	uint8_t b; //!< Blue component
	uint8_t a; //!< Alpha component
} GUM_Color;

#define GUM_COLOR_TYPE (GBL_TYPEID(GUM_Color)) //!< Returns the GUM_Color Type UUID

//! \cond
GblType GUM_Color_type(void);
//! \endcond

GBL_DECLS_END

#endif // GUM_COLOR_H
