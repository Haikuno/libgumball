#include <gumball/types/gumball_event.h>

GblType GUM_Event_type(void) {
	static GblType type = GBL_INVALID_TYPE;

	if (type == GBL_INVALID_TYPE) {
		type = GblType_register(GblQuark_internStatic("GUM_Event"),
							 	GBL_EVENT_TYPE,
							 	&(static GblTypeInfo){.classSize    = sizeof(GUM_EventClass),
													  .instanceSize = sizeof(GUM_Event)},
								GBL_TYPE_FLAG_TYPEINFO_STATIC);
	}

	return type;
}
