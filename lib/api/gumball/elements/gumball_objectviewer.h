#ifndef GUM_OBJECTVIEWER_H
#define GUM_OBJECTVIEWER_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__objectviewer_8h.html

/*!  \file
 *   \ref        GUM_ObjectViewer "GUM_ObjectViewer data structure and hierarchy graph"
 *   \ingroup    elements
 *
 *   TODO: document

 *   \author       2026 Agustín Bellagamba
 *   \copyright    MIT License
*/

#include "gumball_container.h"

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_OBJECTVIEWER_TYPE             (GBL_TYPEID      (GUM_ObjectViewer))           //!< Returns the GUM_ObjectViewer Type UUID
#define GUM_OBJECTVIEWER(self)            (GBL_CAST        (GUM_ObjectViewer, self))     //!< Casts an instance of a compatible element to a GUM_ObjectViewer
#define GUM_OBJECTVIEWER_CLASS(klass)     (GBL_CLASS_CAST  (GUM_ObjectViewer, klass))    //!< Casts a  class    of a compatible element to a GUM_ObjectViewerClass
#define GUM_OBJECTVIEWER_CLASSOF(self)    (GBL_CLASSOF     (GUM_ObjectViewer, self))     //!< Casts an instance of a compatible element to a GUM_ObjectViewerClass
//! @}

#define GBL_SELF_TYPE                GUM_ObjectViewer

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_ObjectViewer);

/*!  \struct  GUM_ObjectViewerClass
 *   \extends GUM_ContainerClass
 *   \brief   GUM_ObjectViewer structure
 *
 *    GUM_ObjectViewerClass derives from GUM_ContainerClass, adding nothing new.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_ObjectViewer, GUM_Container)

/*!  \class   GUM_ObjectViewer
 *   \extends GUM_Container
 *   \brief   An element to reflect over properties of any object.
 *
*/

/*!  \name  Properties
 *   \brief Properties you can set/get at or after creation.
 *   \note  You can also set/get properties from parent classes (see \ref GUM_ObjectViewer).
 *    @{
*/
GBL_INSTANCE_DERIVE(GUM_ObjectViewer, GUM_Container)
    GblObject* pObject; //!< A pointer to the object to be displayed. Default value is nullptr
GBL_INSTANCE_END
//! @}

GBL_PROPERTIES(GUM_ObjectViewer,
    (object, GBL_GENERIC, (READ, WRITE), GBL_OBJECT_TYPE)
)

//! \cond
// GBL_SIGNALS(GUM_ObjectViewer,
//     // TODO: signals?
// )

GblType GUM_ObjectViewer_type(void);
//! \endcond

//! Returns a new GUM_ObjectViewer. Optionally takes in a list of Name/Value pairs for properties
#define GUM_ObjectViewer_create(/* propertyName, propertyValue */ ...) GBL_NEW(GUM_ObjectViewer __VA_OPT__(,) __VA_ARGS__)

//! Sets the object whose properties are displayed by the Object Viewer.
GBL_EXPORT GBL_RESULT GUM_ObjectViewer_setObject(GBL_SELF, GblObject* pObject) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_OBJECTVIEWER_H
