// List of all resource types

#ifndef _RESOURCES_H_
#define _RESOURCES_H_

#include <queue>
#include <string>
#include "Resource.h"
#include "TestResource.h"
#include "WebResource.h"

using namespace std;

// Update this function to create new Resource type
inline Resource *ConstructResource(resource_t type, string *serial = NULL) {
	Resource *r;
	switch (type) {
	case RESOURCE_TEST:
		r = new TestResource();
		break;
	case RESOURCE_WEB:
		r = new WebResource();
		break;
	default:
		r = NULL;
	}
	if (r && serial && r->deserialize(serial)) {
		delete r;
		r = NULL;
	}
	return r;
}

// Add similar function to create new Resource type
inline TestResource *Resource2TestResource(Resource *resource) {
	return dynamic_cast<TestResource*>(resource);
}

inline WebResource *Resource2WebResource(Resource *resource) {
	return dynamic_cast<WebResource*>(resource);
}

// Helper methods for SWIG

inline void ResourceQueuePush(queue<Resource*> *queue, Resource *resource) {
	queue->push(resource);
}

inline Resource *ResourceQueuePop(queue<Resource*> *queue) {
	Resource *resource = queue->front();
	queue->pop();
	return resource;
}

inline int ResourceQueueSize(queue<Resource*> *queue) {
	return queue->size();
}

#endif
