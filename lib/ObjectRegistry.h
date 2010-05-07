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
#include <tr1/unordered_map>
#include <log4cxx/logger.h>
#include "common.h"

class Object;

using namespace std;

class ObjectRegistry {
public:
	ObjectRegistry() {};
	~ObjectRegistry() {};

	bool registerObject(Object *obj);
	bool unregisterObject(const char *id);
	Object *getObject(const char *id);
	vector<string> *getIds();

	char *getObjectValue(const char *id, const char *name);
	bool setObjectValue(const char *id, const char *name, const char *value);

private:
	std::tr1::unordered_map<string, Object*> objects;

	static log4cxx::LoggerPtr logger;
};

#endif
