
#include <stdio.h>
#include <string.h>
#include "TestProtobufResource.h"

using namespace std;

log4cxx::LoggerPtr TestProtobufResource::logger(log4cxx::Logger::getLogger("resources.TestProtobufResource"));

ProtobufResource *TestProtobufResource::Clone() {
	return new TestProtobufResource(*this);
}

void TestProtobufResource::Clear() {
	Resource::Clear();
	r.Clear();
}

string TestProtobufResource::toString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "[TPR %d %d] %s", getId(), isSetFlag(DELETED) ? -1 : getStatus(), getStr().c_str());
	return buf;
}

template <class T>
ResourceFieldInfoT<T>::ResourceFieldInfoT(const string &name) {
	if (name == "id") {
		type = INT;
		get_u.i = &TestProtobufResource::getId;
		set_u.i = &TestProtobufResource::setId;
		clear_all = NULL;
	} else if (name == "status") {
		type = INT;
		get_u.i = &TestProtobufResource::getStatus;
		set_u.i = &TestProtobufResource::setStatus;
		clear_all = NULL;
	} else if (name == "str") {
		type = STRING;
		get_u.s = &TestProtobufResource::getStr;
		set_u.s = &TestProtobufResource::setStr;
		clear_all = NULL;
	}
}
