/**
 * Create resource: load shared libraries automatically.
 */

#include <config.h>

#include <assert.h>
#include "Resources.h"
#include "LibraryLoader.h"

log4cxx::LoggerPtr Resources::logger(log4cxx::Logger::getLogger("lib.processing_engine.Resources"));

PlainLock Resources::lock;
std::tr1::unordered_map<string, int> Resources::name2id;
std::tr1::unordered_map<int, Resource *(*)()> Resources::id2create;

Resources resources;

// Must be called with lock locked
// Either name or id must be non-NULL/0
int Resources::LoadResourceLibrary(const char *name, int id) {
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

Resource *Resources::CreateResource(int id) {
	if (id < 0)
		return NULL;
	lock.Lock();
	std::tr1::unordered_map<int, Resource *(*)()>::iterator iter = id2create.find(id);
	lock.Unlock();
	if (iter == id2create.end()) {
		if (!Resources::LoadResourceLibrary(NULL, id))
			return NULL;
		iter = id2create.find(id);
		assert(iter != id2create.end());
	}
	return (*iter->second)();
}

int Resources::Name2Id(const char *name) {
	lock.Lock();
	std::tr1::unordered_map<string, int>::iterator iter = name2id.find(name);
	if (iter != name2id.end()) {
		lock.Unlock();
		return iter->second;
	}
	if (!Resources::LoadResourceLibrary(name, -1)) {
		lock.Unlock();
		LOG4CXX_ERROR(logger, "Invalid Resource name: " << name);
		return -1;
	}
	iter = name2id.find(name);
	assert(iter != name2id.end());
	lock.Unlock();

	return iter->second;
}
