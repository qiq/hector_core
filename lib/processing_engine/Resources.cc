/**
 * Create resource: load shared libraries automatically.
 */

#include <config.h>

#include "Resources.h"
#include "LibraryLoader.h"

log4cxx::LoggerPtr Resources::logger(log4cxx::Logger::getLogger("lib.processing_engine.Resources"));

Lock Resources::lock;
std::tr1::unordered_map<string, int> Resources::name2id;
std::tr1::unordered_map<int, Resource *(*)()> Resources::id2create;

Resources resources;

Resource *Resources::CreateResource(int id) {
	if (id < 0)
		return NULL;
	lock.lock();
	std::tr1::unordered_map<int, Resource *(*)()>::iterator iter = id2create.find(id);
	lock.unlock();
	if (iter == id2create.end())
		return NULL;
	return (*iter->second)();
}

int Resources::Name2Id(const char *name) {
	lock.lock();
	std::tr1::unordered_map<string, int>::iterator iter = name2id.find(name);
	if (iter != name2id.end()) {
		lock.unlock();
		return iter->second;
	}
	// try to load library
	char s[1024];
	snprintf(s, sizeof(s), "%s.la", name);
	Resource *(*create)() = (Resource*(*)())LibraryLoader::loadLibrary(s, "create");
	if (!create) {
		lock.unlock();
		LOG4CXX_ERROR(logger, "Invalid Resource name: " << name);
		return -1;
	}

	// get id and save it into name2id
	Resource *r = (*create)();
	int id = r->getType();
	name2id[name] = id;
	delete r;

	// save constructor reference
	id2create[id] = create;

	lock.unlock();
	return id;
}
