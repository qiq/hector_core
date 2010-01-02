/**
 * Register all objects, so that we can call getValue() and setValue() on them
 * There is no need to lock registry, because values are added only during init
 * phase (single thread), later we only read the registry.
 */
#ifndef _OBJECT_REGISTRY_H_
#define _OBJECT_REGISTRY_H_

#include <config.h>

#include <string>
#include <vector>
#include <ext/hash_map>
#include <log4cxx/logger.h>
#include "common.h"

class Object;

using namespace std;
namespace stdext = ::__gnu_cxx;

class ObjectRegistry {
	stdext::hash_map<string, Object*, string_hash> objects;

	static log4cxx::LoggerPtr logger;
public:
	ObjectRegistry() {};
	~ObjectRegistry() {};

	void registerObject(Object *obj);
	bool unregisterObject(const char *id);
	Object *getObject(const char *id);
	vector<string> *getIds();

	char *getObjectValue(const char *id, const char *name);
	bool setObjectValue(const char *id, const char *name, const char *value);
};

#endif
