/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCES_TEST_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_RESOURCES_TEST_RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>
#include "Resource.h"
#include "ResourceFieldInfo.h"

class TestResource : public Resource {
public:
	TestResource() {};
	~TestResource() {};
	// create copy of a resource
	Resource *Clone();
	// save and restore resource
	std::string *Serialize();
	bool Deserialize(const char *data, int size);
	// return ResourceInfo describing one field
	ResourceFieldInfo *getFieldInfo(const char *name);
	// type id of a resource (to be used by Resources::CreateResource(typeid))
	int getTypeId();
	// type string of a resource
	const char *getTypeStr();
	// module prefix (e.g. Hector for Hector::TestResource)
	const char *getModuleStr();
	// used by queues in case there is limit on queue size
	int getSize();
	// return string representation of the resource (e.g. for debugging purposes)
	std::string toString(Object::LogLevel logLevel);

	void setStr(const std::string &s);
	const std::string &getStr();

	static const int typeId = 1;

protected:
	std::string str;

	static log4cxx::LoggerPtr logger;
};

inline ResourceFieldInfo *TestResource::getFieldInfo(const char *name) {
	return new ResourceFieldInfoT<TestResource>(name);
}

inline int TestResource::getTypeId() {
	return typeId;
}

inline const char *TestResource::getTypeStr() {
	return "TestResource";
}

inline const char *TestResource::getModuleStr() {
	return "Hector";
}

inline const std::string &TestResource::getStr() {
	return str;
}

inline void TestResource::setStr(const std::string &str) {
	this->str = str;
}

#endif
