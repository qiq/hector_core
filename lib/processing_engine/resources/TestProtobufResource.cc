
#include <stdio.h>
#include <string.h>
#include "TestProtobufResource.h"

using namespace std;

log4cxx::LoggerPtr TestProtobufResource::logger(log4cxx::Logger::getLogger("resources.TestProtobufResource"));

TestProtobufResource::TestProtobufResource() {
	r.set_id(0);
	r.set_status(0);
}

ProtobufResource *TestProtobufResource::Clone() {
	return new TestProtobufResource(*this);
}

int TestProtobufResource::getSize() {
	return 1;
}

string *TestProtobufResource::toString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "TestProtobufResource [%d, %d]: %s", this->getId(), this->getStatus(), this->getStr().c_str());
	return new string(buf);
}

TestProtobufResourceFieldInfo::TestProtobufResourceFieldInfo(const string &name) {
	if (name == "id") {
		type = INT;
		get_u.i = &TestProtobufResource::getId;
		set_u.i = &TestProtobufResource::setId;
		clear_u.c = NULL;
	} else if (name == "status") {
		type = INT;
		get_u.i = &TestProtobufResource::getStatus;
		set_u.i = &TestProtobufResource::setStatus;
		clear_u.c = NULL;
	} else if (name == "str") {
		type = STRING;
		get_u.s = &TestProtobufResource::getStr;
		set_u.s = &TestProtobufResource::setStr;
		clear_u.c = NULL;
	}
}

const string &TestProtobufResourceFieldInfo::getString(Resource *resource) {
	assert(resource->getTypeId() == TestProtobufResource::typeId);
	return get_u.s ? (static_cast<TestProtobufResource*>(resource)->*get_u.s)() : empty_string;
}

int TestProtobufResourceFieldInfo::getInt(Resource *resource) {
	assert(resource->getTypeId() == TestProtobufResource::typeId);
	return get_u.i ? (static_cast<TestProtobufResource*>(resource)->*get_u.i)() : -1;
}

void TestProtobufResourceFieldInfo::setString(Resource *resource, const string &value) {
	assert(resource->getTypeId() == TestProtobufResource::typeId);
	if (set_u.s)
		(static_cast<TestProtobufResource*>(resource)->*set_u.s)(value);
}

void TestProtobufResourceFieldInfo::setInt(Resource *resource, int value) {
	assert(resource->getTypeId() == TestProtobufResource::typeId);
	if (set_u.i)
		(static_cast<TestProtobufResource*>(resource)->*set_u.i)(value);
}

void TestProtobufResourceFieldInfo::clear(Resource *resource) {
	assert(resource->getTypeId() == TestProtobufResource::typeId);
	if (clear_u.c)
		(static_cast<TestProtobufResource*>(resource)->*clear_u.c)();
}
