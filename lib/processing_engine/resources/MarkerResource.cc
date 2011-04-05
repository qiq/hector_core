
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "MarkerResource.h"

using namespace std;

log4cxx::LoggerPtr MarkerResource::logger(log4cxx::Logger::getLogger("resources.MarkerResource"));

Resource *MarkerResource::Clone() {
	return new MarkerResource(*this);
}

void MarkerResource::Clear() {
	Resource::Clear();
	mark = 0;
}

string *MarkerResource::Serialize() {
	char s[1024];
	snprintf(s, sizeof(s), "%d\n%d\n%d\n", id, status, mark);
	return new string(s);
}

bool MarkerResource::Deserialize(const char *data, int size) {
	char s[1024+1];
	strncpy(s, data, size > 1024 ? 1024 : size);
	s[size] = '\0';
	int notused;	// do not load (change) resource id
	if (sscanf(s, "%d\n%d\n%d\n", &notused, &status, &mark) != 3) {
		LOG4CXX_ERROR_R(logger, this, "Cannot deserialize MarkerResource: " << s);
		return false;
	}
	return true;
}

string MarkerResource::ToString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "[TR %d %d] %d", id, IsSetFlag(DELETED) ? -1 : status, mark);
	return buf;
}

vector<ResourceAttrInfo*> *MarkerResource::GetAttrInfoList() {
	vector<ResourceAttrInfo*> *result = new vector<ResourceAttrInfo*>();
	ResourceAttrInfoT<MarkerResource> *ai;

	ai = new ResourceAttrInfoT<MarkerResource>(typeId);
	ai->InitInt("id", &MarkerResource::GetId, &MarkerResource::SetId);
	result->push_back(ai);

	ai = new ResourceAttrInfoT<MarkerResource>(typeId);
	ai->InitInt("status", &MarkerResource::GetStatus, &MarkerResource::SetStatus);
	result->push_back(ai);

	ai = new ResourceAttrInfoT<MarkerResource>(typeId);
	ai->InitInt("mark", &MarkerResource::GetMark, &MarkerResource::SetMark);
	result->push_back(ai);

	return result;
}
