
#include <stdio.h>
#include <string.h>
#include "TestProtobufResource.h"

using namespace std;

log4cxx::LoggerPtr TestProtobufResource::logger(log4cxx::Logger::getLogger("resources.TestProtobufResource"));

ProtobufResource *TestProtobufResource::Clone() {
	return new TestProtobufResource(*this);
}

int TestProtobufResource::getSize() {
	return 1;
}

string TestProtobufResource::toString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "[TPR %d %d] %s", getId(), isStatusDeleted() ? -1 : getStatus(), getStr().c_str());
	return buf;
}

template <class T>
ResourceFieldInfoT<T>::ResourceFieldInfoT(const string &name) {
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
