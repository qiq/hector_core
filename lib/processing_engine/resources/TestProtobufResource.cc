
#include <stdio.h>
#include <string.h>
#include "TestProtobufResource.h"

using namespace std;

vector<ResourceAttrInfoT<TestProtobufResource> > TestProtobufResource::info;
tr1::unordered_map<string, ResourceAttrInfoT<TestProtobufResource>*> TestProtobufResource::infoMap;
bool TestProtobufResource::init = TestProtobufResource::Init();	// static init
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

bool TestProtobufResource::Init() {
	ResourceAttrInfoT<TestProtobufResource> ai;

	ai.InitInt("id", &TestProtobufResource::getId, &TestProtobufResource::setId);
	info.push_back(ai);
	ai.InitInt("status", &TestProtobufResource::getStatus, &TestProtobufResource::setStatus);
	info.push_back(ai);
	ai.InitString("str", &TestProtobufResource::getStr, &TestProtobufResource::setStr);
	info.push_back(ai);

	for (vector<ResourceAttrInfoT<TestProtobufResource> >::iterator iter = info.begin(); iter != info.end(); ++iter)
		infoMap[iter->getName()] = &(*iter);
	return true;
}

ResourceAttrInfo *TestProtobufResource::GetAttrInfo(const char *name) {
	tr1::unordered_map<std::string, ResourceAttrInfoT<TestProtobufResource>*>::iterator iter = infoMap.find(name);
	return iter->second;
}
