/**
 * This type of resource may be emmited as an indicator of a processing stage.
 * E.g. that all resources have been read.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCES_MARKER_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_RESOURCES_MARKER_RESOURCE_H_

#include <config.h>

#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include "Resource.h"

class ResourceAttrInfo;
class ResourceInputStream;
class ResourceOutputStream;

class MarkerResource : public Resource {
public:
	MarkerResource() {};
	~MarkerResource() {};
	// create copy of a resource
	Resource *Clone();
	// Clear current resource (as would delete + new do)
	void Clear();
	// save and restore resource
	bool Serialize(ResourceOutputStream &output);
	bool Deserialize(ResourceInputStream &input);
	// return ResourceAttrInfo describing one field
	std::vector<ResourceAttrInfo*> *GetAttrInfoList();
	// type id of a resource (to be used by Resources::AcquireResource(typeid))
	int GetTypeId();
	// type string of a resource
	const char *GetTypeString(bool terse = false);
	// object name (for construction of an object or a reference)
	const char *GetObjectName();
	// used by queues in case there is limit on queue size
	int GetSize();
	// return string representation of the resource (e.g. for debugging purposes)
	std::string ToString(Object::LogLevel logLevel);

	void SetMark(int mark);
	int GetMark();

	static bool IsInstance(Resource *resource);

protected:
	static const int typeId = 3;

	int mark;

	static log4cxx::LoggerPtr logger;
};

inline int MarkerResource::GetTypeId() {
	return typeId; 
}

inline const char *MarkerResource::GetTypeString(bool terse) {
	return terse ? "MR" : "MarkerResource";
}

inline const char *MarkerResource::GetObjectName() {
	return "MarkerResource";
}

inline int MarkerResource::GetSize() {
	return sizeof(int);
}

inline int MarkerResource::GetMark() {
	return mark;
}

inline void MarkerResource::SetMark(int mark) {
	this->mark = mark;
}

inline bool MarkerResource::IsInstance(Resource *resource) {
	return resource->GetTypeId() == typeId;
}

#endif
