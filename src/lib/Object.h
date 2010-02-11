// basic object interface, get/set values


#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <config.h>

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include "ObjectRegistry.h"
#include "Lock.h"

class Object {
public:
	Object(ObjectRegistry *objects, const char *id);
	~Object();
	void DoLock();
	void DoUnlock();
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

inline void Object::DoLock() {
	lock.lock();
}

inline void Object::DoUnlock() {
	lock.unlock();
}

inline char *Object::getValue(const char *name) {
	char *result;
	DoLock();
	result = getValueSync(name);
	DoUnlock();
	return result;
}

inline bool Object::setValue(const char *name, const char *value) {
	bool result;
	DoLock();
	result = setValueSync(name, value);
	DoUnlock();
	return result;
}

inline vector<string> *Object::listNames() {
	vector<string> *result;
	DoLock();
	result = listNamesSync();
	DoUnlock();
	return result;
}

inline void Object::SaveCheckpoint(const char *path, const char *id) {
	DoLock();
	SaveCheckpointSync(path, id);
	DoUnlock();
}

inline void Object::RestoreCheckpoint(const char *path, const char *id) {
	DoLock();
	RestoreCheckpointSync(path, id);
	DoUnlock();
}

#endif
