/**
 * Register all objects, so that we can call getValue() and setValue() on them
 * There is no need to lock registry, because values are added only during init
 * phase (single thread), later we only read the registry.
 */
#ifndef _LIB_OBJECT_REGISTRY_H_
#define _LIB_OBJECT_REGISTRY_H_

#include <config.h>

#include <string>
#include <vector>
#include <tr1/unordered_map>
#include <log4cxx/logger.h>
#include "common.h"

class Object;

class ObjectRegistry {
public:
	ObjectRegistry() {};
	~ObjectRegistry() {};

	bool RegisterObject(Object *obj);
	bool UnregisterObject(const char *id);
	Object *GetObject(const char *id);
	std::vector<std::string> *GetIds();

	char *GetObjectProperty(const char *id, const char *name);
	bool SetObjectProperty(const char *id, const char *name, const char *value);

private:
	std::tr1::unordered_map<std::string, Object*> objects;

	static log4cxx::LoggerPtr logger;
};

#endif
