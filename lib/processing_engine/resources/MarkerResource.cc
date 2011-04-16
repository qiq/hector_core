
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "MarkerResource.h"
#include "ResourceAttrInfoT.h"
#include "ResourceInputStream.h"
#include "ResourceOutputStream.h"

using namespace std;

#ifndef WRAPPER

log4cxx::LoggerPtr MarkerResource::logger(log4cxx::Logger::getLogger("resources.MarkerResource"));
MarkerResourceInfo MarkerResource::resourceInfo;

MarkerResourceInfo::MarkerResourceInfo() {
	SetTypeId(3);
	SetTypeString("MarkerResource");
	SetTypeStringTerse("MR");
	SetObjectName("MarkerResource");

	vector<ResourceAttrInfo*> *l = new vector<ResourceAttrInfo*>();
	ResourceAttrInfoT<MarkerResource> *ai;

	ai = new ResourceAttrInfoT<MarkerResource>(GetTypeId());
	ai->InitInt32("id", &MarkerResource::GetId, &MarkerResource::SetId);
	l->push_back(ai);

	ai = new ResourceAttrInfoT<MarkerResource>(GetTypeId());
	ai->InitInt32("status", &MarkerResource::GetStatus, &MarkerResource::SetStatus);
	l->push_back(ai);

	ai = new ResourceAttrInfoT<MarkerResource>(GetTypeId());
	ai->InitInt32("mark", &MarkerResource::GetMark, &MarkerResource::SetMark);
	l->push_back(ai);

	SetAttrInfoList(l);
}

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

#else

extern "C" Resource* create() {
	return (Resource*)new MarkerResource();
}

#endif
