
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "MarkerResource.h"
#include "ResourceAttrInfoT.h"
#include "ResourceInputStream.h"
#include "ResourceOutputStream.h"

using namespace std;

log4cxx::LoggerPtr MarkerResource::logger(log4cxx::Logger::getLogger("resources.MarkerResource"));

Resource *MarkerResource::Clone() {
	return new MarkerResource(*this);
}

void MarkerResource::Clear() {
	Resource::Clear();
	mark = 0;
}

bool MarkerResource::Serialize(ResourceOutputStream &output) {
	output.WriteVarint32((uint32_t)mark);
	return true;
}

bool MarkerResource::Deserialize(ResourceInputStream &input) {
	return input.ReadVarint32((uint32_t*)&mark);
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
