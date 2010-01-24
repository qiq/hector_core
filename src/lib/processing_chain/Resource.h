/**
 * Class representing a resource, basic item of processing.
 * It uses Google Protocol Buffers to de/serialize.
 */

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>

using namespace std;

class Resource {
protected:
	Resource *anotherResource;

	static log4cxx::LoggerPtr logger;
public:
	Resource();
	virtual ~Resource();
	virtual int getId() = 0;
	virtual void setId(int id) = 0;
	virtual int getStatus() = 0;
	virtual void setStatus(int status) = 0;
	virtual int getSize() = 0;
//	virtual int getType() = 0;

	Resource *getAnotherResource();
	Resource *setAnotherResource(Resource *resource);

	virtual string *serialize() = 0;
	virtual bool deserialize(string *s) = 0;
};

inline Resource::Resource() {
	anotherResource = NULL;
}

inline Resource::~Resource() {
	delete anotherResource;
	anotherResource = NULL;
}

inline Resource *Resource::getAnotherResource() {
	return anotherResource;
}

inline Resource *Resource::setAnotherResource(Resource *resource) {
	Resource *tmp = anotherResource;
	anotherResource = resource;
	return tmp;
}

#endif
