
#include <config.h>

#include <assert.h>
#include "Resource.h"
#include "ResourceInputStream.h"
#include "ResourceOutputStream.h"

using namespace std;

log4cxx::LoggerPtr Resource::logger(log4cxx::Logger::getLogger("Resource"));

ResourceRegistry *Resource::registry = NULL;
string Resource::empty;

Resource::Resource() {
	flags = 0;
	SetId(registry->NextResourceId());
	status = 0;
	attachedResource = NULL;
}

Resource::Resource(const Resource &r) : status(r.status), flags(r.flags), attachedResource(r.attachedResource) {
	SetId(registry->NextResourceId());
}

void Resource::Clear() {
	flags = 0;
	status = 0;
	id = 0;
	attachedResource = NULL;
}

bool Resource::Serialize(Resource *resource, ResourceOutputStream &stream, bool saveType, bool saveIdStatus) {
	if (saveType) {
		int type = resource->GetTypeId();
		if (saveIdStatus)
			type |= 0x128;
		stream.WriteVarint32(type);
		if (saveIdStatus) {
			stream.WriteVarint32(resource->GetId());
			stream.WriteVarint32(resource->GetStatus());
		}
	}
	if (!resource->Serialize(stream))
		return false;
	return true;
}

Resource *Resource::Deserialize(ResourceInputStream &stream, int resourceType, int *totalSize) {
	long current;
	if (totalSize)
		current = stream.ByteCount();
	bool readIdStatus = false;
	if (!resourceType) {
		uint32_t type;
		if (!stream.ReadVarint32(&type))
			return NULL;
		if (type & 0x128)
			readIdStatus = true;
		resourceType = type & 0x127;
	}
	Resource *r = registry->AcquireResource(resourceType);
	if (!r) {
		LOG4CXX_ERROR(logger, "Cannot create resource of type: " << resourceType);
		return NULL;
	}
	if (readIdStatus) {
		uint32_t id;
		uint32_t status;
		if (!stream.ReadVarint32(&id) || !stream.ReadVarint32(&status)) {
			delete r;
			return NULL;
		}
		r->SetId(id);
		r->SetStatus(status);
	}

	if (!r->Deserialize(stream)) {
		registry->ReleaseResource(r);
		return NULL;
	}
	if (totalSize)
		*totalSize = stream.ByteCount()-current;
	return r;
}

