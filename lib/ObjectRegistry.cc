// 
#include <config.h>

#include "Object.h"
#include "ObjectRegistry.h"

log4cxx::LoggerPtr ObjectRegistry::logger(log4cxx::Logger::getLogger("lib.ObjectRegistry"));

bool ObjectRegistry::registerObject(Object *obj) {
	const char *id = obj->getId();
	std::tr1::unordered_map<string, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		LOG4CXX_ERROR(logger, "Object " << id << " already defined");
		return false;
	}
	objects[id] = obj;
	return true;
}

bool ObjectRegistry::unregisterObject(const char *id) {
	std::tr1::unordered_map<string, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		objects.erase(id);
		return true;
	}
	return false;
}

Object *ObjectRegistry::getObject(const char *id) {
	std::tr1::unordered_map<string, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		return iter->second;
	}
	return NULL;
}

vector<string> *ObjectRegistry::getIds() {
	vector<string> *result = new vector<string>();
	for (std::tr1::unordered_map<string, Object*>::iterator iter = objects.begin(); iter != objects.end(); ++iter) {
		result->push_back(iter->first);
	}
	return result;
}

char *ObjectRegistry::getObjectValue(const char *id, const char *name) {
	std::tr1::unordered_map<string, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		Object *obj = iter->second;
		return obj->getValue(name);
	}
	return NULL;
}

bool ObjectRegistry::setObjectValue(const char *id, const char *name, const char *value) {
	std::tr1::unordered_map<string, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		return iter->second->setValue(name, value);
	}
	return false;
}
