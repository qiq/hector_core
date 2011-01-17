
#include <config.h>

#include <assert.h>
#include "LibraryLoader.h"
#include "Resource.h"
#include "ResourceRegistry.h"

using namespace std;

struct ResourceInfo {
	Resource *(*create)();
	vector<Resource*> available;
};

log4cxx::LoggerPtr ResourceRegistry::logger(log4cxx::Logger::getLogger("ResourceRegistry"));

ResourceRegistry::~ResourceRegistry() {
	for (tr1::unordered_map<int, ResourceInfo*>::iterator iter = id2info.begin(); iter != id2info.end(); ++iter) {
		vector<Resource*> *v = &iter->second->available;
		for (vector<Resource*>::iterator iter2 = v->begin(); iter2 != v->end(); ++iter2) {
			delete *iter2;
		}
		delete iter->second;
	}
}

// Must be called with lock locked
// Either name or id must be non-NULL/0
int ResourceRegistry::LoadResourceLibrary(const char *name, int id) {
	// try to load library
	char s[1024];
	if (name)
		snprintf(s, sizeof(s), "%s.la", name);
	else
		snprintf(s, sizeof(s), "%d.la", id);
	Resource *(*create)() = (Resource*(*)())LibraryLoader::loadLibrary(s, "create");
	if (!create) {
		if (name) {
			LOG4CXX_ERROR(logger, "Invalid Resource name: " << name);
		} else {
			LOG4CXX_ERROR(logger, "Invalid Resource Id: " << id);
		}
		return false;
	}
	// get id and save it into name2id
	Resource *r = (*create)();
	id = r->getTypeId();
	name = r->getTypeStr();
	name2id[name] = id;
	ResourceInfo *info = new ResourceInfo();
	// save constructor reference and store first available resource
	info->create = create;
	info->available.push_back(r);
	id2info[id] = info;
	return true;
}

int ResourceRegistry::NameToId(const char *name) {
	translateLock.Lock();
	tr1::unordered_map<string, int>::iterator iter = name2id.find(name);
	if (iter != name2id.end()) {
		translateLock.Unlock();
		return iter->second;
	}
	if (!ResourceRegistry::LoadResourceLibrary(name, -1)) {
		translateLock.Unlock();
		LOG4CXX_ERROR(logger, "Invalid Resource name: " << name);
		return -1;
	}
	iter = name2id.find(name);
	assert(iter != name2id.end());
	int result = iter->second;
	translateLock.Unlock();
	return result;
}

Resource *ResourceRegistry::AcquireResource(int id) {
	if (id < 0)
		return NULL;
	translateLock.Lock();
	tr1::unordered_map<int, ResourceInfo*>::iterator iter = id2info.find(id);
	if (iter == id2info.end()) {
		if (!ResourceRegistry::LoadResourceLibrary(NULL, id)) {
			translateLock.Unlock();
			return NULL;
		}
		iter = id2info.find(id);
		assert(iter != id2info.end());
	}
	Resource *result;
	if (iter->second->available.size() > 0) {
		result = iter->second->available.back();
		result->setId(Resource::NextResourceId());
		iter->second->available.pop_back();
	} else {
 		result = iter->second->create();
	}
	translateLock.Unlock();
	return result;
}

#define AVAILABLE_SIZE_MAX 10000
void ResourceRegistry::ReleaseResource(Resource *resource) {
	if (!resource)
		return;
	resource->Clear();
	translateLock.Lock();
	tr1::unordered_map<int, ResourceInfo*>::iterator iter = id2info.find(resource->getTypeId());
	if (iter == id2info.end()) {
		if (!ResourceRegistry::LoadResourceLibrary(NULL, resource->getTypeId()))
			return;
		iter = id2info.find(resource->getTypeId());
		assert(iter != id2info.end());
	}
	if (iter->second->available.size() < AVAILABLE_SIZE_MAX)
		iter->second->available.push_back(resource);
	else
		delete resource;
	translateLock.Unlock();
}

