
#include <config.h>

#include <assert.h>
#include "LibraryLoader.h"
#include "Resource.h"

using namespace std;

log4cxx::LoggerPtr Resource::logger(log4cxx::Logger::getLogger("Resource"));

PlainLock Resource::translateLock;
tr1::unordered_map<string, int> Resource::name2id;
tr1::unordered_map<int, Resource *(*)()> Resource::id2create;

PlainLock Resource::idLock;
int Resource::nextId = 0;

Resource::Resource() {
	flags = 0;
	idLock.Lock();
	setId(nextId++);
	idLock.Unlock();
	status = 0;
	attachedResource = NULL;
}

Resource::Resource(const Resource &r) : status(r.status), attachedResource(r.attachedResource) {
	idLock.Lock();
	setId(nextId++);
	idLock.Unlock();
}

// Must be called with lock locked
// Either name or id must be non-NULL/0
int Resource::LoadResourceLibrary(const char *name, int id) {
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
	delete r;
	// save constructor reference
	id2create[id] = create;
	return true;
}

Resource *Resource::CreateResource(int id) {
	if (id < 0)
		return NULL;
	translateLock.Lock();
	std::tr1::unordered_map<int, Resource *(*)()>::iterator iter = id2create.find(id);
	if (iter == id2create.end()) {
		if (!Resource::LoadResourceLibrary(NULL, id))
			return NULL;
		iter = id2create.find(id);
		assert(iter != id2create.end());
	}
	translateLock.Unlock();
	Resource *result = (*iter->second)();
	return result;
}

int Resource::NameToId(const char *name) {
	translateLock.Lock();
	std::tr1::unordered_map<string, int>::iterator iter = name2id.find(name);
	if (iter != name2id.end()) {
		translateLock.Unlock();
		return iter->second;
	}
	if (!Resource::LoadResourceLibrary(name, -1)) {
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
