// 
#include <config.h>

#include "Object.h"
#include "ObjectRegistry.h"

log4cxx::LoggerPtr ObjectRegistry::logger(log4cxx::Logger::getLogger("lib.ObjectRegistry"));

void ObjectRegistry::registerObject(Object *obj) {
	objects[obj->getId()] = obj;
}


bool ObjectRegistry::unregisterObject(const char *id) {
	stdext::hash_map<string, Object*, string_hash>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		objects.erase(id);
		return true;
	}
	return false;
}

Object *ObjectRegistry::getObject(const char *id) {
	stdext::hash_map<string, Object*, string_hash>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		return iter->second;
	}
	return NULL;
}

const char *ObjectRegistry::getObjectValue(const char *id, const char *name) {
	stdext::hash_map<string, Object*, string_hash>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		Object *obj = iter->second;
		return obj->getValue(name);
	}
	return NULL;
}

bool ObjectRegistry::setObjectValue(const char *id, const char *name, const char *value) {
	stdext::hash_map<string, Object*, string_hash>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		return iter->second->setValue(name, value);
	}
	return false;
}
