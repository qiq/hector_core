/**
 * Class representing a resource, basic item of processing.
 * It uses Google Protocol Buffers to de/serialize.
 */

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>
#include "resources/ResourceList.h"

using namespace std;

class Resource {
protected:
	static log4cxx::LoggerPtr logger;
public:
	Resource();
	virtual ~Resource();
	virtual Resource *Clone() = 0;
	virtual int getId() = 0;
	virtual void setId(int id) = 0;
	virtual int getStatus() = 0;
	virtual void setStatus(int status) = 0;
	virtual int getSize() = 0;
	virtual resource_t getType() = 0;

	virtual string *serialize() = 0;
	virtual bool deserialize(string *s) = 0;
};

inline Resource::Resource() {
}

inline Resource::~Resource() {
}

#endif
