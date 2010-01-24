// List of all resource types

#include "Resource.h"
#include "WebResource.h"

// Update this function to create new Resource type
inline Resource *ConstructResource(int type) {
	switch (type) {
	case 0:
		return NULL;
	case 1:
		return new WebResource();
	default:
		return NULL;
	}
}

// Add similar function to create new Resource type
inline WebResource *Resource2WebResource(Resource *resource) {
	return dynamic_cast<WebResource*>(resource);
}
