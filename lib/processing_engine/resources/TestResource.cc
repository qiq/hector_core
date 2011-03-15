
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "TestResource.h"

using namespace std;

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

string TestResource::ToString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "[TR %d %d] %s", id, IsSetFlag(DELETED) ? -1 : status, str.c_str());
	return buf;
}

vector<ResourceAttrInfo*> *TestResource::GetAttrInfoList() {
	vector<ResourceAttrInfo*> *result = new vector<ResourceAttrInfo*>();
	ResourceAttrInfoT<TestResource> *ai;

	ai = new ResourceAttrInfoT<TestResource>(typeId);
	ai->InitInt("id", &TestResource::GetId, &TestResource::SetId);
	result->push_back(ai);

	ai = new ResourceAttrInfoT<TestResource>(typeId);
	ai->InitInt("status", &TestResource::GetStatus, &TestResource::SetStatus);
	result->push_back(ai);

	ai = new ResourceAttrInfoT<TestResource>(typeId);
	ai->InitString("str", &TestResource::GetStr, &TestResource::SetStr);
	result->push_back(ai);

	return result;
}
