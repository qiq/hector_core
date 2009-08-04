// basic object interface, get/set values


#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <config.h>

#include <stdlib.h>
#include <string.h>

class Object {
	char *id;
public:
	Object();
	~Object();
	void setId(const char *id);
	const char *getId();
	virtual const char *getValue(const char *name) = 0;
	virtual bool setValue(const char *name, const char *value) = 0;
};

inline Object::Object() {
	id = NULL;
}

inline Object::~Object() {
	free(id);
}

inline void Object::setId(const char *id) {
	this->id = strdup(id);
}

inline const char *Object::getId() {
	return id;
}

#endif
