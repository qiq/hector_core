/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCES_TEST_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_RESOURCES_TEST_RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>
#include "Resource.h"

class TestResource : public Resource {
public:
	TestResource();
	~TestResource() {};
	// create copy of a resource
	Resource *Clone();
	// type id of a resource (to be used by Resources::CreateResource(typeid))
	int getTypeId();
	// type string of a resource
	const char *getTypeStr();
	// module prefix (e.g. Hector for Hector::TestResource)
	const char *getModuleStr();
	// id should be unique across all resources
	int getId();
	void setId(int id);
	// status may be tested in Processor to select target queue
	int getStatus();
	void setStatus(int status);
	// save and restore resource
	std::string *Serialize();
	bool Deserialize(std::string *s);
	// used by queues in case there is limit on queue size
	int getSize();
	// return string representation of the resource (e.g. for debugging purposes)
	char *toString(Object::LogLevel logLevel);

	void setStr(const char *s);
	const char *getStr();

	static const int typeId = 1;

protected:
	int id;
	int status;
	std::string str;

	static log4cxx::LoggerPtr logger;
};

inline int TestResource::getTypeId() {
	return typeId;
}

inline const char *TestResource::getTypeStr() {
	return "TestResource";
}

inline const char *TestResource::getModuleStr() {
	return "Hector";
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
