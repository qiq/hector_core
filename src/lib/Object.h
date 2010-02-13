// basic object interface, get/set values


#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <config.h>

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include "common.h"
#include "ObjectRegistry.h"
#include "Lock.h"

#define LOG_TRACE(logger, ...) LOG4CXX_TRACE(logger, getId() << ": " << __VA_ARGS__)
#define LOG_DEBUG(logger, ...) LOG4CXX_DEBUG(logger, getId() << ": " << __VA_ARGS__)
#define LOG_INFO(logger, ...) LOG4CXX_INFO(logger, getId() << ": " << __VA_ARGS__)
#define LOG_ERROR(logger, ...) LOG4CXX_ERROR(logger, getId() << ": " << __VA_ARGS__)
#define LOG_FATAL(logger, ...) LOG4CXX_FATAL(logger, getId() << ": " << __VA_ARGS__)

class Object {
public:
	Object(ObjectRegistry *objects, const char *id);
	~Object();
	void ObjectLock();
	void ObjectUnlock();
	const char *getId();
	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	vector<string> *listNames();
	void SaveCheckpoint(const char *path, const char *id);
	void RestoreCheckpoint(const char *path, const char *id);

protected:
	ObjectRegistry *objects;
	char *id;
	Lock lock;

	virtual char *getValueSync(const char *name);
	virtual bool setValueSync(const char *name, const char *value);
	virtual vector<string> *listNamesSync();

	virtual void SaveCheckpointSync(const char *path, const char *id);
	virtual void RestoreCheckpointSync(const char *path, const char *id);
};

inline Object::Object(ObjectRegistry *objects, const char *id) {
	this->objects = objects;
	this->id = strdup(id);
	if (objects)
		objects->registerObject(this);
}

inline Object::~Object() {
	if (objects)
		objects->unregisterObject(id);
	free(id);
}

inline const char *Object::getId() {
	return this->id;
}

inline void Object::ObjectLock() {
	lock.lock();
}

inline void Object::ObjectUnlock() {
	lock.unlock();
}

inline char *Object::getValue(const char *name) {
	char *result;
	ObjectLock();
	result = getValueSync(name);
	ObjectUnlock();
	return result;
}

inline bool Object::setValue(const char *name, const char *value) {
	bool result;
	ObjectLock();
	result = setValueSync(name, value);
	ObjectUnlock();
	return result;
}

inline vector<string> *Object::listNames() {
	vector<string> *result;
	ObjectLock();
	result = listNamesSync();
	ObjectUnlock();
	return result;
}

inline void Object::SaveCheckpoint(const char *path, const char *id) {
	ObjectLock();
	SaveCheckpointSync(path, id);
	ObjectUnlock();
}

inline void Object::RestoreCheckpoint(const char *path, const char *id) {
	ObjectLock();
	RestoreCheckpointSync(path, id);
	ObjectUnlock();
}

#endif
