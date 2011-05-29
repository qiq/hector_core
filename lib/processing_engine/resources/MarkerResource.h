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

class MarkerResourceInfo : public ResourceInfo {
public:
	MarkerResourceInfo();
};

class MarkerResource : public Resource {
public:
	MarkerResource(): mark(0) {};
	~MarkerResource() {};
	// create copy of a resource
	Resource *Clone();
	// Clear current resource (as would delete + new do)
	void Clear();
	// save and restore resource
	bool Serialize(ResourceOutputStream &output);
	bool Deserialize(ResourceInputStream &input, bool headerOnly);
	bool Skip(ResourceInputStream &input);
	// used by queues in case there is limit on queue size
	int GetSize();
	// get info about this resource
	ResourceInfo *GetResourceInfo();
	// return string representation of the resource (e.g. for debugging purposes)
	std::string ToString(Object::LogLevel logLevel);

	void SetMark(int mark);
	int GetMark();

	static bool IsInstance(Resource *resource);

protected:
	int mark;

	static MarkerResourceInfo resourceInfo;
	static log4cxx::LoggerPtr logger;
};


inline int MarkerResource::GetSize() {
	return sizeof(int);
}

inline ResourceInfo *MarkerResource::GetResourceInfo() {
	return &MarkerResource::resourceInfo;
}

inline int MarkerResource::GetMark() {
	return mark;
}

inline void MarkerResource::SetMark(int mark) {
	this->mark = mark;
}

inline bool MarkerResource::IsInstance(Resource *resource) {
	return resource->GetTypeId() == resourceInfo.GetTypeId();
}

#endif
