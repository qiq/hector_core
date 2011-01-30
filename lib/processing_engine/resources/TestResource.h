/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCES_TEST_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_RESOURCES_TEST_RESOURCE_H_

#include <config.h>

#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include "Resource.h"
#include "ResourceFieldInfo.h"
#include "ResourceAttrInfoT.h"

class TestResource : public Resource {
public:
	TestResource() {};
	~TestResource() {};
	// create copy of a resource
	Resource *Clone();
	// Clear current resource (as would delete + new do)
	void Clear();
	// save and restore resource
	std::string *Serialize();
	bool Deserialize(const char *data, int size);
	// return ResourceAttrInfo describing one field
	ResourceAttrInfo *GetAttrInfo(const char *name);
	// type id of a resource (to be used by Resources::AcquireResource(typeid))
	int getTypeId();
	// type string of a resource
	const char *getTypeStr();
	const char *getTypeStrShort();
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

	static std::vector<ResourceAttrInfoT<TestResource> > info;
	static std::tr1::unordered_map<std::string, ResourceAttrInfoT<TestResource>*> infoMap;

	static bool init;
	static bool Init();

	static log4cxx::LoggerPtr logger;
};

//inline ResourceFieldInfo *TestResource::getFieldInfo(const char *name) {
//	return new ResourceFieldInfoT<TestResource>(name);
//}

inline int TestResource::getTypeId() {
	return typeId;
}

inline const char *TestResource::getTypeStr() {
	return "TestResource";
}

inline const char *TestResource::getTypeStrShort() {
	return "TR";
}

inline const char *TestResource::getModuleStr() {
	return "Hector";
}

inline int TestResource::getSize() {
	return 1;
}

inline const std::string &TestResource::getStr() {
	return str;
}

inline void TestResource::setStr(const std::string &str) {
	this->str = str;
}

#endif
