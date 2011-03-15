// 
#include <config.h>

#include "Object.h"
#include "ObjectRegistry.h"

using namespace std;

log4cxx::LoggerPtr ObjectRegistry::logger(log4cxx::Logger::getLogger("lib.ObjectRegistry"));

bool ObjectRegistry::RegisterObject(Object *obj) {
	const char *id = obj->GetId();
	tr1::unordered_map<string, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		LOG4CXX_ERROR(logger, "Object " << id << " already defined");
		return false;
	}
	objects[id] = obj;
	return true;
}

bool ObjectRegistry::UnregisterObject(const char *id) {
	tr1::unordered_map<string, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		objects.erase(id);
		return true;
	}
	return false;
}

Object *ObjectRegistry::GetObject(const char *id) {
	tr1::unordered_map<string, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		return iter->second;
	}
	return NULL;
}

vector<string> *ObjectRegistry::GetIds() {
	vector<string> *result = new vector<string>();
	for (tr1::unordered_map<string, Object*>::iterator iter = objects.begin(); iter != objects.end(); ++iter) {
		result->push_back(iter->first);
	}
	return result;
}

char *ObjectRegistry::GetObjectValue(const char *id, const char *name) {
	tr1::unordered_map<string, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		Object *obj = iter->second;
		return obj->GetValue(name);
	}
	return NULL;
}

bool ObjectRegistry::SetObjectValue(const char *id, const char *name, const char *value) {
	tr1::unordered_map<string, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		return iter->second->SetValue(name, value);
	}
	return false;
}
