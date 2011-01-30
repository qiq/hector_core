
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "TestResource.h"

using namespace std;

vector<ResourceAttrInfoT<TestResource> > TestResource::info;
tr1::unordered_map<string, ResourceAttrInfoT<TestResource>*> TestResource::infoMap;
bool TestResource::init = TestResource::Init();	// static init
log4cxx::LoggerPtr TestResource::logger(log4cxx::Logger::getLogger("resources.TestResource"));

Resource *TestResource::Clone() {
	return new TestResource(*this);
}

void TestResource::Clear() {
	Resource::Clear();
	str.clear();
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
	s[size] = '\0';
	char buf[size > 1024 ? 1024 : size];
	int notused;	// do not load (change) resource id
	if (sscanf(s, "%d\n%d\n%1023s\n", &notused, &status, (char*)&buf) != 3) {
		LOG4CXX_ERROR_R(logger, this, "Cannot deserialize TestResource: " << s);
		return false;
	}
	str.assign(buf);
	return true;
}

string TestResource::toString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "[TR %d %d] %s", id, isSetFlag(DELETED) ? -1 : status, str.c_str());
	return buf;
}

bool TestResource::Init() {
	ResourceAttrInfoT<TestResource> ai;

	ai.InitInt("id", &TestResource::getId, &TestResource::setId);
	info.push_back(ai);
	ai.InitInt("status", &TestResource::getStatus, &TestResource::setStatus);
	info.push_back(ai);
	ai.InitString("str", &TestResource::getStr, &TestResource::setStr);
	info.push_back(ai);

	for (vector<ResourceAttrInfoT<TestResource> >::iterator iter = info.begin(); iter != info.end(); ++iter)
		infoMap[iter->getName()] = &(*iter);
	return true;
}

ResourceAttrInfo *TestResource::GetAttrInfo(const char *name) {
	tr1::unordered_map<std::string, ResourceAttrInfoT<TestResource>*>::iterator iter = infoMap.find(name);
	return iter->second;
}
