// basic object interface, get/set values


#ifndef _LIB_OBJECT_H_
#define _LIB_OBJECT_H_

#include <config.h>

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include "common.h"
#include "ObjectRegistry.h"
#include "RWLock.h"

#define LOG_TRACE(logger, ...) { LOG4CXX_TRACE(logger, getId() << ": " << __VA_ARGS__) }
#define LOG_DEBUG(logger, ...) { LOG4CXX_DEBUG(logger, getId() << ": " << __VA_ARGS__) }
#define LOG_INFO(logger, ...) { LOG4CXX_INFO(logger, getId() << ": " << __VA_ARGS__) }
#define LOG_ERROR(logger, ...) { LOG4CXX_ERROR(logger, getId() << ": " << __VA_ARGS__) }
#define LOG_FATAL(logger, ...) { LOG4CXX_FATAL(logger, getId() << ": " << __VA_ARGS__) }

class Object {
public:
	Object(ObjectRegistry *objects, const char *id);
	Object(ObjectRegistry *objects, const char *id, int index);
	~Object();
	ObjectRegistry *getObjectRegistry();
	void ObjectLockRead();
	void ObjectLockWrite();
	void ObjectUnlock();
	const char *getId();
	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	std::vector<std::string> *listNames();
	void SaveCheckpoint(const char *path, const char *id);
	void RestoreCheckpoint(const char *path, const char *id);

	void log_trace(const char *s);
	void log_debug(const char *s);
	void log_info(const char *s);
	void log_error(const char *s);
	void log_fatal(const char *s);

protected:
	ObjectRegistry *objects;
	char *id;
	RWLock rwlock;

	virtual char *getValueSync(const char *name);
	virtual bool setValueSync(const char *name, const char *value);
	virtual std::vector<std::string> *listNamesSync();

	virtual void SaveCheckpointSync(const char *path, const char *id);
	virtual void RestoreCheckpointSync(const char *path, const char *id);

	static log4cxx::LoggerPtr logger;
};

inline Object::Object(ObjectRegistry *objects, const char *id) {
	this->objects = objects;
	this->id = strdup(id);
	if (objects)
		objects->registerObject(this);
}

inline Object::Object(ObjectRegistry *objects, const char *id, int index) {
	this->objects = objects;
	char s[1024];
	snprintf(s, sizeof(s), "%s[%d]", id, index);
	this->id = strdup(s);
	if (objects)
		objects->registerObject(this);
}

inline Object::~Object() {
	if (objects)
		objects->unregisterObject(id);
	free(id);
}
inline ObjectRegistry *Object::getObjectRegistry() {
	return objects;
}

inline const char *Object::getId() {
	return id;
}

inline void Object::ObjectLockRead() {
	rwlock.LockRead();
}

inline void Object::ObjectLockWrite() {
	rwlock.LockWrite();
}

inline void Object::ObjectUnlock() {
	rwlock.Unlock();
}

inline char *Object::getValue(const char *name) {
	char *result;
	ObjectLockRead();
	result = getValueSync(name);
	ObjectUnlock();
	return result;
}

inline bool Object::setValue(const char *name, const char *value) {
	bool result;
	ObjectLockWrite();
	result = setValueSync(name, value);
	ObjectUnlock();
	return result;
}

inline std::vector<std::string> *Object::listNames() {
	std::vector<std::string> *result;
	ObjectLockRead();
	result = listNamesSync();
	ObjectUnlock();
	return result;
}

inline void Object::SaveCheckpoint(const char *path, const char *id) {
	ObjectLockWrite();
	SaveCheckpointSync(path, id);
	ObjectUnlock();
}

inline void Object::RestoreCheckpoint(const char *path, const char *id) {
	ObjectLockWrite();
	RestoreCheckpointSync(path, id);
	ObjectUnlock();
}

inline void Object::log_trace(const char *s) {
	LOG_TRACE(logger, s);
}

inline void Object::log_debug(const char *s) {
	LOG_DEBUG(logger, s);
}

inline void Object::log_info(const char *s) {
	LOG_INFO(logger, s);
}

inline void Object::log_error(const char *s) {
	LOG_ERROR(logger, s);
}

inline void Object::log_fatal(const char *s) {
	LOG_FATAL(logger, s);
}

#endif
