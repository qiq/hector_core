// List of all resource types

#ifndef _RESOURCES_H_
#define _RESOURCES_H_

#include <config.h>

#include <queue>
#include <string>
#include <tr1/unordered_map>
#include <log4cxx/logger.h>
#include "Lock.h"
#include "Resource.h"

using namespace std;

class Resources {
public:
	Resources() {};
	~Resources() {};

	static Resource *CreateResource(int id);
	static int Name2Id(const char *name);
private:
	static Lock lock;
	static std::tr1::unordered_map<string, int> name2id;
	static std::tr1::unordered_map<int, Resource *(*)()> id2create;
	static log4cxx::LoggerPtr logger;
};

#if 0
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
#endif

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
