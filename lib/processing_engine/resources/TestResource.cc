
#include <stdio.h>
#include <string.h>
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

bool TestResource::Deserialize(string *s) {
	char buf[1024];
	if (sscanf(s->c_str(), "%d\n%d\n%1023s\n", &id, &status, (char*)&buf) != 3) {
		LOG4CXX_ERROR(logger, "Cannot deserialize TestResource: " << s);
		return false;
	}
	str.assign(buf);
	return true;
}

char *TestResource::toString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "TestResource [%d, %d]: %s", id, status, str.c_str());
	return strdup(buf);
}

TestResource::TestResourceFieldInfo TestResource::getFieldInfo(const char *name) {
	TestResource::TestResourceFieldInfo result;
	if (!strcmp(name, "id")) {
		result.type = INT;
		result.get.i = &TestResource::getId;
		result.set.i = &TestResource::setId;
		return result;
	} else if (!strcmp(name, "status")) {
		result.type = INT;
		result.get.i = &TestResource::getStatus;
		result.set.i = &TestResource::setStatus;
		return result;
	} else if (!strcmp(name, "str")) {
		result.type = STRING;
		result.get.s = &TestResource::getStr;
		result.set.s = &TestResource::setStr;
		return result;
	}
}

