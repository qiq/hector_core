
#include <config.h>

#include <assert.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include "Resource.h"
#include "ProtobufResource.h"

using namespace std;

log4cxx::LoggerPtr Resource::logger(log4cxx::Logger::getLogger("Resource"));

ResourceRegistry Resource::registry;

PlainLock Resource::idLock;
int Resource::nextId = 0;

Resource::Resource() {
	flags = 0;
	idLock.Lock();
	setId(nextId++);
	idLock.Unlock();
	status = 0;
	attachedResource = NULL;
}

Resource::Resource(const Resource &r) : flags(r.flags), status(r.status), attachedResource(r.attachedResource) {
	idLock.Lock();
	setId(nextId++);
	idLock.Unlock();
}

void Resource::Clear() {
	flags = 0;
	status = 0;
	id = 0;
	attachedResource = NULL;
}

int Resource::NextResourceId() {
	idLock.Lock();
	int next = nextId++;
	idLock.Unlock();
	return next;
}

bool Resource::Serialize(Resource *resource, google::protobuf::io::CodedOutputStream *stream) {
	char buffer[5];
	*(uint8_t*)(buffer+4) = resource->getTypeId();
	if (resource->isProtobufResource()) {
		ProtobufResource *pr = static_cast<ProtobufResource*>(resource);
		*(uint32_t*)buffer = pr->GetSerializedSize();
		stream->WriteRaw(buffer, 5);
		if (!pr->SerializeWithCachedSize(stream))
			return false;
	} else {
		string *serial = resource->Serialize();
		if (!serial)
			return false;
		*(uint32_t*)buffer = serial->length();
		stream->WriteRaw(buffer, 5);
		stream->WriteRaw(serial->data(), *(uint32_t*)buffer);
		delete serial;
	}
	return true;
}

Resource *Resource::Deserialize(google::protobuf::io::CodedInputStream *stream, int *totalSize) {
	char buffer[5];
	if (!stream->ReadRaw(buffer, 5))
		return NULL;
	uint32_t size = *(uint32_t*)buffer;
	uint8_t typeId = *(uint8_t*)(buffer+4);
	Resource *r = registry.AcquireResource(typeId);
	if (!r) {
		LOG4CXX_ERROR(logger, "Cannot create resource of type: " << typeId);
		return NULL;
	}
	if (r->isProtobufResource()) {
		ProtobufResource *pr = static_cast<ProtobufResource*>(r);
		google::protobuf::io::CodedInputStream::Limit l = stream->PushLimit(size);
		pr->Deserialize(stream);
		stream->PopLimit(l);
	} else {
		char *data = (char*)malloc(size);
		if (!stream->ReadRaw(data, size)) {
			free(data);
			registry.ReleaseResource(r);
			return NULL;
		}
		if (!r->Deserialize(data, size)) {
			free(data);
			registry.ReleaseResource(r);
			return NULL;
		}
		free(data);
	}
	if (totalSize)
		*totalSize = size+5;
	return r;
}
