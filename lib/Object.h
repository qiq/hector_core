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
	ObjectRegistry *GetObjectRegistry();
	void ObjectLockRead();
	void ObjectLockWrite();
	void ObjectUnlock();
	const char *getId();
	char *LockGetValue(const char *name);
	bool LockSetValue(const char *name, const char *value);
	std::vector<std::string> *LockListNames();
	bool LockSaveCheckpoint(const char *path);
	bool LockRestoreCheckpoint(const char *path);

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

	bool setLogLevel(const char *logLevel);

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

	virtual char *GetValue(const char *name);
	virtual bool SetValue(const char *name, const char *value);
	virtual bool IsInitOnly(const char *name);
	virtual std::vector<std::string> *ListNames();

	virtual bool SaveCheckpoint(const char *path);
	virtual bool RestoreCheckpoint(const char *path);

	const char *GetLogLevelStr(log4cxx::LoggerPtr logger);

	log4cxx::LoggerPtr logger;
};

inline Object::Object(ObjectRegistry *objects, const char *id) {
	this->objects = objects;
	this->id = strdup(id);
	if (objects)
		objects->RegisterObject(this);
	logger = log4cxx::Logger::getLogger(id);
}

inline Object::Object(ObjectRegistry *objects, const char *id, int index) {
	this->objects = objects;
	char s[1024];
	snprintf(s, sizeof(s), "%s[%d]", id, index);
	this->id = strdup(s);
	if (objects)
		objects->RegisterObject(this);
	logger = log4cxx::Logger::getLogger(id);
}

inline Object::~Object() {
	if (objects)
		objects->UnregisterObject(id);
	free(id);
}

inline ObjectRegistry *Object::GetObjectRegistry() {
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

inline char *Object::LockGetValue(const char *name) {
	if (!strcmp(name, "logLevel"))
		return strdup(this->GetLogLevelStr(this->logger));
	// no need to lock
	if (IsInitOnly(name))
		return GetValue(name);
	char *result;
	ObjectLockRead();
	result = GetValue(name);
	ObjectUnlock();
	return result;
}

inline bool Object::LockSetValue(const char *name, const char *value) {
	if (!strcmp(name, "logLevel"))
		return this->setLogLevel(value);
	bool result;
	if (IsInitOnly(name))
		return false;
	ObjectLockWrite();
	result = SetValue(name, value);
	ObjectUnlock();
	return result;
}

inline std::vector<std::string> *Object::LockListNames() {
	std::vector<std::string> *result;
	ObjectLockRead();
	result = ListNames();
	ObjectUnlock();
	result->push_back("logLevel");
	return result;
}

inline bool Object::LockSaveCheckpoint(const char *path) {
	ObjectLockWrite();
	bool result = SaveCheckpoint(path);
	ObjectUnlock();
	return result;
}

inline bool Object::LockRestoreCheckpoint(const char *path) {
	ObjectLockWrite();
	bool result = RestoreCheckpoint(path);
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
