#include <gumball/ifaces/gumball_iresource.h>
#include <gumball/core/gumball_logger.h>

#define GUM_IRESOURCE_QUARK_FIELD_NAME "GUM_IResource_quark"
#define GUM_IRESOURCE_VALUE_FIELD_NAME "GUM_IResource_value"

GBL_EXPORT GUM_IResource *GUM_IResource_ref(GUM_IResource* pResource) {
	return GUM_IRESOURCE(GBL_REF(pResource));
}

GBL_EXPORT GblRefCount GUM_IResource_unref(GUM_IResource* pResource) {
	GUM_LOG_DEBUG_SCOPE("GUM_IResource_unref() called...") {
		if (!pResource) {
			GUM_LOG_ERROR("Tried to unref a null resource!");
			GBL_SCOPE_EXIT;
		}

		if (GblBox_refCount(GBL_BOX(pResource)) <= 1) {
			GUM_LOG_ERROR("Tried to unref a resource that is not being used!");
			GUM_LOG_ERROR("Use GUM_Manager_unload() instead.");
		}

		GUM_LOG_DEBUG("Resource unreffed!");
	}

	return GBL_UNREF(pResource);
}

GBL_EXPORT void *GUM_IResource_data(const GUM_IResource *pResource) {
	return (void*)GblBox_field(GBL_BOX(pResource), GblQuark_fromStatic(GUM_IRESOURCE_VALUE_FIELD_NAME));
}

GBL_EXPORT void GUM_IResource_setData(GUM_IResource *pResource, void *pValue) {
	GblBox_setField(GBL_BOX(pResource), GblQuark_fromStatic(GUM_IRESOURCE_VALUE_FIELD_NAME), (uintptr_t)pValue);
}

static GBL_RESULT GUM_IResource_setValue_(GUM_IResource *pResource, void *pValue) {
	GblBox_setField(GBL_BOX(pResource), GblQuark_fromStatic(GUM_IRESOURCE_VALUE_FIELD_NAME), (uintptr_t)pValue);
	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_IResource_quark_(const GUM_IResource *pResource, GblQuark *pQuark) {
	*pQuark = (GblQuark)GblBox_field(GBL_BOX(pResource), GblQuark_fromStatic(GUM_IRESOURCE_QUARK_FIELD_NAME));
	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_IResource_setQuark_(GUM_IResource *pResource, GblQuark quark) {
	GblBox_setField(GBL_BOX(pResource), GblQuark_fromStatic(GUM_IRESOURCE_QUARK_FIELD_NAME), (uintptr_t)quark);
	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_IResourceClass_init_(GblClass *pClass, const void *pData) {
	GBL_UNUSED(pData);

	GUM_IRESOURCE_CLASS(pClass)->pFnSetValue  = GUM_IResource_setValue_;
	GUM_IRESOURCE_CLASS(pClass)->pFnQuark     = GUM_IResource_quark_;
	GUM_IRESOURCE_CLASS(pClass)->pFnSetQuark  = GUM_IResource_setQuark_;

	return GBL_RESULT_SUCCESS;
}

GblType GUM_IResource_type(void) {
	static GblType type 		  =  GBL_INVALID_TYPE;
    static GblType dependencies[] = {GBL_INVALID_TYPE};

	if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        dependencies[0] = GBL_BOX_TYPE;
		type = GblType_register(GblQuark_internStatic("GUM_IResource"),
							 	GBL_INTERFACE_TYPE,
							 	&(static GblTypeInfo){.classSize       = sizeof(GUM_IResourceClass),
													  .pFnClassInit    = GUM_IResourceClass_init_,
											          .pDependencies   = dependencies,
													  .dependencyCount = 1},
							 	GBL_TYPE_FLAG_TYPEINFO_STATIC);
	}

	return type;
}