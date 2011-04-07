
#include <stdio.h>
#include <string.h>
#include "TestProtobufResource.h"
#include "ResourceAttrInfoT.h"
#include "ResourceInputStream.h"
#include "ResourceOutputStream.h"

using namespace std;

log4cxx::LoggerPtr TestProtobufResource::logger(log4cxx::Logger::getLogger("resources.TestProtobufResource"));

Resource *TestProtobufResource::Clone() {
	return new TestProtobufResource(*this);
}

void TestProtobufResource::Clear() {
	Resource::Clear();
	r.Clear();
}

bool TestProtobufResource::Serialize(ResourceOutputStream &output) {
	output.WriteVarint32(r.ByteSize());
	r.SerializeWithCachedSizes(output.GetCodedOutputStream());
	return true;
}

bool TestProtobufResource::Deserialize(ResourceInputStream &input) {
	uint32_t size;
	if (!input.ReadVarint32(&size))
                return false;
	google::protobuf::io::CodedInputStream::Limit l = input.PushLimit(size);
	bool result = r.ParseFromCodedStream(input.GetCodedInputStream());
	input.PopLimit(l);
	return result;
}

string TestProtobufResource::ToString(Object::LogLevel logLevel) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "[TPR %d %d] %s", GetId(), IsSetFlag(DELETED) ? -1 : GetStatus(), GetStr().c_str());
	return buf;
}

vector<ResourceAttrInfo*> *TestProtobufResource::GetAttrInfoList() {
	vector<ResourceAttrInfo*> *result = new vector<ResourceAttrInfo*>();
	ResourceAttrInfoT<TestProtobufResource> *ai;

	ai = new ResourceAttrInfoT<TestProtobufResource>(typeId);
	ai->InitInt("id", &TestProtobufResource::GetId, &TestProtobufResource::SetId);
	result->push_back(ai);

	ai = new ResourceAttrInfoT<TestProtobufResource>(typeId);
	ai->InitInt("status", &TestProtobufResource::GetStatus, &TestProtobufResource::SetStatus);
	result->push_back(ai);

	ai = new ResourceAttrInfoT<TestProtobufResource>(typeId);
	ai->InitString("str", &TestProtobufResource::GetStr, &TestProtobufResource::SetStr);
	result->push_back(ai);

	return result;
}
