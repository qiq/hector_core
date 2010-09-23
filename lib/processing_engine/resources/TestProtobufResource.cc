
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
