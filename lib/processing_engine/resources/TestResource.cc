
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "TestResource.h"

using namespace std;

log4cxx::LoggerPtr TestResource::logger(log4cxx::Logger::getLogger("resources.TestResource"));

TestResource::TestResource() {
	id = 0;
	status = 0;
}

Resource *TestResource::Clone() {
	return new TestResource(*this);
}

int TestResource::getSize() {
	return 1;
}

string *TestResource::Serialize() {
	string *strcopy = new string(str);
	size_t idx;
	while ((idx = strcopy->find('\n')) != string::npos) {
		strcopy->replace(idx, 1, " ");
	}
	char s[1024];
	snprintf(s, sizeof(s), "%d\n%d\n%s\n", id, status, strcopy->c_str());
	delete strcopy;
	return new string(s);
}

bool TestResource::Deserialize(const char *data, int size) {
	char s[1024+1];
	strncpy(s, data, size > 1024 ? 1024 : size);
	s[1024] = '\0';
	char buf[1024];
	if (sscanf(s, "%d\n%d\n%1023s\n", &id, &status, (char*)&buf) != 3) {
		LOG4CXX_ERROR(logger, "Cannot deserialize TestResource: " << s);
		return false;
	}
	str.assign(buf);
	return true;
}

string *TestResource::toString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "TestResource [%d, %d]: %s", id, status, str.c_str());
	return new string(buf);
}

TestResourceFieldInfo::TestResourceFieldInfo(const string &name) {
	if (name == "id") {
		type = INT;
		get_u.i = &TestResource::getId;
		set_u.i = &TestResource::setId;
		clear_u.c = NULL;
	} else if (name == "status") {
		type = INT;
		get_u.i = &TestResource::getStatus;
		set_u.i = &TestResource::setStatus;
		clear_u.c = NULL;
	} else if (name == "str") {
		type = STRING;
		get_u.s = &TestResource::getStr;
		set_u.s = &TestResource::setStr;
		clear_u.c = NULL;
	}
}

const std::string &TestResourceFieldInfo::getString(Resource *resource) {
	assert(resource->getTypeId() == TestResource::typeId);
	return get_u.s ? (static_cast<TestResource*>(resource)->*get_u.s)() : empty_string;
}

int TestResourceFieldInfo::getInt(Resource *resource) {
	assert(resource->getTypeId() == TestResource::typeId);
	return get_u.i ? (static_cast<TestResource*>(resource)->*get_u.i)() : -1;
}

void TestResourceFieldInfo::setString(Resource *resource, const std::string &value) {
	assert(resource->getTypeId() == TestResource::typeId);
	if (set_u.s)
		(static_cast<TestResource*>(resource)->*set_u.s)(value);
}

void TestResourceFieldInfo::setInt(Resource *resource, int value) {
	assert(resource->getTypeId() == TestResource::typeId);
	if (set_u.i)
		(static_cast<TestResource*>(resource)->*set_u.i)(value);
}

void TestResourceFieldInfo::clear(Resource *resource) {
	assert(resource->getTypeId() == TestResource::typeId);
	if (clear_u.c)
		(static_cast<TestResource*>(resource)->*clear_u.c)();
}
