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

#define LOG_TRACE(o, ...) { LOG4CXX_TRACE(o->getLogger(), o->getId() << ": " << __VA_ARGS__) }
#define LOG_DEBUG(o, ...) { LOG4CXX_DEBUG(o->getLogger(), o->getId() << ": " << __VA_ARGS__) }
#define LOG_INFO(o, ...) { LOG4CXX_INFO(o->getLogger(), o->getId() << ": " << __VA_ARGS__) }
#define LOG_ERROR(o, ...) { LOG4CXX_ERROR(o->getLogger(), o->getId() << ": " << __VA_ARGS__) }
#define LOG_FATAL(o, ...) { LOG4CXX_FATAL(o->getLogger(), o->getId() << ": " << __VA_ARGS__) }

class Object {
public:
	Object(ObjectRegistry *objects, const char *id);
	Object(ObjectRegistry *objects, const char *id, int index);
	virtual ~Object();
	ObjectRegistry *getObjectRegistry();
	void ObjectLockRead();
	void ObjectLockWrite();
	void ObjectUnlock();
	const char *getId();
	char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
	std::vector<std::string> *listNames();
	bool SaveCheckpoint(const char *path);
	bool RestoreCheckpoint(const char *path);

	log4cxx::LoggerPtr getLogger();

	void log_trace(const char *s);
	void log_trace(const std::string &s);
	void log_debug(const char *s);
	void log_debug(const std::string &s);
	void log_info(const char *s);
	void log_info(const std::string &s);
	void log_error(const char *s);
	void log_error(const std::string &s);
	void log_fatal(const char *s);
	void log_fatal(const std::string &s);

	enum LogLevel {
		TRACE = 0,
		DEBUG =  1,
		INFO = 2,
		ERROR = 3,
		FATAL =  4,
	};

protected:
	ObjectRegistry *objects;
	char *id;
	RWLock rwlock;

	virtual char *getValueSync(const char *name);
	virtual bool setValueSync(const char *name, const char *value);
	virtual bool isInitOnly(const char *name);
	virtual std::vector<std::string> *listNamesSync();

	virtual bool SaveCheckpointSync(const char *path);
	virtual bool RestoreCheckpointSync(const char *path);

	const char *getLogLevelStr(log4cxx::LoggerPtr logger);
	bool setLogLevel(const char *logLevel);

	log4cxx::LoggerPtr logger;
};

inline Object::Object(ObjectRegistry *objects, const char *id) {
	this->objects = objects;
	this->id = strdup(id);
	if (objects)
		objects->registerObject(this);
	logger = log4cxx::Logger::getLogger(id);
}

inline Object::Object(ObjectRegistry *objects, const char *id, int index) {
	this->objects = objects;
	char s[1024];
	snprintf(s, sizeof(s), "%s[%d]", id, index);
	this->id = strdup(s);
	if (objects)
		objects->registerObject(this);
	logger = log4cxx::Logger::getLogger(id);
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
	if (!strcmp(name, "logLevel"))
		return strdup(this->getLogLevelStr(this->logger));
	// no need to lock
	if (isInitOnly(name))
		return getValueSync(name);
	char *result;
	ObjectLockRead();
	result = getValueSync(name);
	ObjectUnlock();
	return result;
}

inline bool Object::setValue(const char *name, const char *value) {
	if (!strcmp(name, "logLevel"))
		return this->setLogLevel(value);
	bool result;
	if (isInitOnly(name))
		return false;
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
	result->push_back("logLevel");
	return result;
}

inline bool Object::SaveCheckpoint(const char *path) {
	ObjectLockWrite();
	bool result = SaveCheckpointSync(path);
	ObjectUnlock();
	return result;
}

inline bool Object::RestoreCheckpoint(const char *path) {
	ObjectLockWrite();
	bool result = RestoreCheckpointSync(path);
	ObjectUnlock();
	return result;
}

inline log4cxx::LoggerPtr Object::getLogger() {
	return logger;
}

inline void Object::log_trace(const char *s) {
	LOG_TRACE(this, s);
}

inline void Object::log_trace(const std::string &s) {
	LOG_TRACE(this, s);
}

inline void Object::log_debug(const char *s) {
	LOG_DEBUG(this, s);
}

inline void Object::log_debug(const std::string &s) {
	LOG_DEBUG(this, s);
}

inline void Object::log_info(const char *s) {
	LOG_INFO(this, s);
}

inline void Object::log_info(const std::string &s) {
	LOG_INFO(this, s);
}

inline void Object::log_error(const char *s) {
	LOG_ERROR(this, s);
}

inline void Object::log_error(const std::string &s) {
	LOG_ERROR(this, s);
}

inline void Object::log_fatal(const char *s) {
	LOG_FATAL(this, s);
}

inline void Object::log_fatal(const std::string &s) {
	LOG_FATAL(this, s);
}

#endif
