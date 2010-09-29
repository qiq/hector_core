
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

char *TestProtobufResource::toString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "TestProtobufResource [%d, %d]: %s", this->getId(), this->getStatus(), this->getStr());
	return strdup(buf);
}

TestProtobufResource::TestProtobufResourceFieldInfo TestProtobufResource::getFieldInfo(const char *name) {
	TestProtobufResource::TestProtobufResourceFieldInfo result;
	if (!strcmp(name, "id")) {
		result.type = INT;
		result.get.i = &TestProtobufResource::getId;
		result.set.i = &TestProtobufResource::setId;
		return result;
	} else if (!strcmp(name, "status")) {
		result.type = INT;
		result.get.i = &TestProtobufResource::getStatus;
		result.set.i = &TestProtobufResource::setStatus;
		return result;
	} else if (!strcmp(name, "str")) {
		result.type = STRING;
		result.get.s = &TestProtobufResource::getStr;
		result.set.s = &TestProtobufResource::setStr;
		return result;
	}
}
