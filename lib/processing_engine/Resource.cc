
#include <config.h>

#include <assert.h>
#include "Resource.h"
#include "ResourceInputStream.h"
#include "ResourceOutputStream.h"

using namespace std;

log4cxx::LoggerPtr Resource::logger(log4cxx::Logger::getLogger("Resource"));

ResourceRegistry *Resource::registry = NULL;
string Resource::empty;

bool Resource::SerializeResource(Resource *resource, ResourceOutputStream &stream, bool saveType, bool saveIdStatus) {
	if (saveType) {
		int type = resource->GetTypeId();
		if (saveIdStatus)
			type |= 0x80;
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

Resource *Resource::DeserializeResource(ResourceInputStream &stream, int resourceType, int *totalSize) {
	long current;
	if (totalSize)
		current = stream.ByteCount();
	bool readIdStatus = false;
	if (!resourceType) {
		uint32_t type;
		if (!stream.ReadVarint32(&type))
			return NULL;
		if (type & 0x80)
			readIdStatus = true;
		resourceType = type & 0x7f;
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

