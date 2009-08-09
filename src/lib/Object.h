// basic object interface, get/set values


#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <config.h>

#include <stdlib.h>
#include <string.h>
#include "ObjectRegistry.h"

class Object {
protected:
	ObjectRegistry *objects;
	char *id;
	
public:
	Object(ObjectRegistry *objects, const char *id);
	~Object();
	const char *getId();
	virtual const char *getValue(const char *name) = 0;
	virtual bool setValue(const char *name, const char *value) = 0;
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

#endif
