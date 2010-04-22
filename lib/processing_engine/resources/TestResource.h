/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 * It uses Google Protocol Buffers to de/serialize.
 */

#ifndef _TEST_RESOURCE_H_
#define _TEST_RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>
#include "Resource.h"

using namespace std;

class TestResource : public Resource {
public:
	TestResource();
	~TestResource() {};
	// create copy of a resource
	Resource *Clone();
	// type id of a resource (to be used by Resources::CreateResource(typeid))
	int getType();
	// id should be unique across all resources
	int getId();
	void setId(int id);
	// status may be tested in Processor to select target queue
	int getStatus();
	void setStatus(int status);
	// save and restore resource
	string *serialize();
	bool deserialize(string *s);
	// used by queues in case there is limit on queue size
	int getSize();

	void setStr(const char *s);
	const char *getStr();

protected:
	int id;
	int status;
	string str;

	static log4cxx::LoggerPtr logger;
};

inline int TestResource::getType() {
	return 1;
}

inline int TestResource::getId() {
	return id;
}

inline void TestResource::setId(int id) {
	this->id = id;
}

inline int TestResource::getStatus() {
	return status;
}

inline void TestResource::setStatus(int status) {
	this->status = status;
}

inline const char *TestResource::getStr() {
	return str.c_str();
}

inline void TestResource::setStr(const char *str) {
	this->str = str;
}

#endif
