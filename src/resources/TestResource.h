/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 * It uses Google Protocol Buffers to de/serialize.
 */

#ifndef _TEST_RESOURCE_H_
#define _TEST_RESOURCE_H_

#include <config.h>

#include <vector>
#include <string>
#include <log4cxx/logger.h>
#include "Resource.h"
#include "TestResource.pb.h"

using namespace std;

class TestResource : public Resource {
protected:
	hector::resources::TestResource r;

	static log4cxx::LoggerPtr logger;
public:
	TestResource();
	~TestResource() {};
	Resource *Clone();
	int getStatus();
	void setStatus(int status);
	int getId();
	void setId(int id);
	int getSize();
	resource_t getType();

	void setStr(const char *s);
	const char *getStr();

	string *serialize();
	bool deserialize(string *s);
};

inline int TestResource::getId() {
	return r.id();
}

inline void TestResource::setId(int id) {
	r.set_id(id);
}

inline int TestResource::getStatus() {
	return r.status();
}

inline void TestResource::setStatus(int status) {
	r.set_status(status);
}

inline resource_t TestResource::getType() {
	return RESOURCE_TEST;
}

inline const char *TestResource::getStr() {
	return r.str().c_str();
}

inline void TestResource::setStr(const char *str) {
	r.set_str(str);
}

#endif
