/**
 * Class representing a resource, basic item of processing.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_H_

#include <config.h>

#include <limits>
#include <string>
#include <log4cxx/logger.h>
#include "Object.h"
#include "PlainLock.h"
#include "ResourceAttrInfo.h"
#include "ResourceRegistry.h"

class ResourceAttrInfo;
class ResourceInputStream;
class ResourceOutputStream;

// static info of a Resource
class ResourceInfo {
public:
	ResourceInfo(): typeId(0), typeString(NULL), typeStringTerse(NULL), objectName(NULL), attrInfoList(NULL) {};
	~ResourceInfo();
	int GetTypeId();
	void SetTypeId(int typeId);
	const char *GetTypeString();
	void SetTypeString(const char *typeString);
	const char *GetTypeStringTerse();
	void SetTypeStringTerse(const char *typeStringTerse);
	const char *GetObjectName();
	void SetObjectName(const char *typeId);
	std::vector<ResourceAttrInfo*> *GetAttrInfoList();
	void SetAttrInfoList(std::vector<ResourceAttrInfo*> *attrInfoList);
private:
	int typeId;		// type id of a resource (to be used by Resources::AcquireResource(typeid))
	char *typeString; 	// type string of a resource
	char *typeStringTerse;	// terse version (e.g. TR for TestResource)
	char *objectName;	// object name (for construction of an object or a reference)
	std::vector<ResourceAttrInfo*> *attrInfoList;	// info about a resource fields
};

class Resource {
public:
	enum Flag {
		DELETED = 1,	// resource will be deleted
		SKIP = 2,	// the rest of processor modules should be skipped, resource is just appended to the queue
	};

	Resource();
	Resource(const Resource &r);
	virtual ~Resource() {};

	// Following methods are expected to be overriden in ancestor Resources
	// (they are made pure virtual)

	// Create copy of a resource
	virtual Resource *Clone() = 0;
	// Clear current resource (as delete + new would do, except id is set to 0)
	virtual void Clear();
	// save and restore resource
	virtual bool Serialize(ResourceOutputStream &output) = 0;
	virtual bool Deserialize(ResourceInputStream &input, bool headerOnly = false) = 0;
	virtual bool Skip(ResourceInputStream &input) = 0;
	// used by queues in case there is limit on queue size, this size may
	// be somewhat arbitrary
	virtual int GetSize() = 0;
	// get info about this resource
	virtual ResourceInfo *GetResourceInfo() = 0;
	// return string representation of the resource (e.g. for debugging purposes)
	virtual std::string ToString(Object::LogLevel = Object::INFO) = 0;

	// Following methods are common to all Resources: id and status are
	// always present in memory, generic API for getting/setting
	// attributes, getting/setting flag and attachedResource

	// id should be unique across all in-memory resources
	virtual int GetId();
	virtual void SetId(int id);
	// status may be tested in Processor to select target queue
	virtual int GetStatus();
	virtual void SetStatus(int status);

	// generic API for getting/setting of attributes
	virtual int32_t GetAttrInt32(const char *name);
	virtual int32_t GetAttrInt32(ResourceAttrInfo *ai);
	virtual uint32_t GetAttrUInt32(const char *name);
	virtual uint32_t GetAttrUInt32(ResourceAttrInfo *ai);
	virtual int64_t GetAttrInt64(const char *name);
	virtual int64_t GetAttrInt64(ResourceAttrInfo *ai);
	virtual uint64_t GetAttrUInt64(const char *name);
	virtual uint64_t GetAttrUInt64(ResourceAttrInfo *ai);
	virtual double GetAttrDouble(const char *name);
	virtual double GetAttrDouble(ResourceAttrInfo *ai);
	virtual const std::string GetAttrString(const char *name);
	virtual const std::string GetAttrString(ResourceAttrInfo *ai);
	virtual const IpAddr GetAttrIpAddr(const char *name);
	virtual const IpAddr GetAttrIpAddr(ResourceAttrInfo *ai);
	virtual int32_t GetAttrArrayInt32(const char *name, int index);
	virtual int32_t GetAttrArrayInt32(ResourceAttrInfo *ai, int index);
	virtual uint32_t GetAttrArrayUInt32(const char *name, int index);
	virtual uint32_t GetAttrArrayUInt32(ResourceAttrInfo *ai, int index);
	virtual int64_t GetAttrArrayInt64(const char *name, int index);
	virtual int64_t GetAttrArrayInt64(ResourceAttrInfo *ai, int index);
	virtual uint64_t GetAttrArrayUInt64(const char *name, int index);
	virtual uint64_t GetAttrArrayUInt64(ResourceAttrInfo *ai, int index);
	virtual double GetAttrArrayDouble(const char *name, int index);
	virtual double GetAttrArrayDouble(ResourceAttrInfo *ai, int index);
	virtual const std::string GetAttrArrayString(const char *name, int index);
	virtual const std::string GetAttrArrayString(ResourceAttrInfo *ai, int index);
	virtual const IpAddr GetAttrArrayIpAddr(const char *name, int index);
	virtual const IpAddr GetAttrArrayIpAddr(ResourceAttrInfo *ai, int index);
	virtual int32_t GetAttrHashInt32(const char *name, const std::string &index);
	virtual int32_t GetAttrHashInt32(ResourceAttrInfo *ai, const std::string &index);
	virtual uint32_t GetAttrHashUInt32(const char *name, const std::string &index);
	virtual uint32_t GetAttrHashUInt32(ResourceAttrInfo *ai, const std::string &index);
	virtual int64_t GetAttrHashInt64(const char *name, const std::string &index);
	virtual int64_t GetAttrHashInt64(ResourceAttrInfo *ai, const std::string &index);
	virtual uint64_t GetAttrHashUInt64(const char *name, const std::string &index);
	virtual uint64_t GetAttrHashUInt64(ResourceAttrInfo *ai, const std::string &index);
	virtual double GetAttrHashDouble(const char *name, const std::string &index);
	virtual double GetAttrHashDouble(ResourceAttrInfo *ai, const std::string &index);
	virtual const std::string GetAttrHashString(const char *name, const std::string &index);
	virtual const std::string GetAttrHashString(ResourceAttrInfo *ai, const std::string &index);
	virtual const IpAddr GetAttrHashIpAddr(const char *name, const std::string &index);
	virtual const IpAddr GetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index);

	virtual void SetAttrInt32(const char *name, int32_t value);
	virtual void SetAttrInt32(ResourceAttrInfo *ai, int32_t value);
	virtual void SetAttrUInt32(const char *name, uint32_t value);
	virtual void SetAttrUInt32(ResourceAttrInfo *ai, uint32_t value);
	virtual void SetAttrInt64(const char *name, int64_t value);
	virtual void SetAttrInt64(ResourceAttrInfo *ai, int64_t value);
	virtual void SetAttrUInt64(const char *name, uint64_t value);
	virtual void SetAttrUInt64(ResourceAttrInfo *ai, uint64_t value);
	virtual void SetAttrDouble(const char *name, double value);
	virtual void SetAttrDouble(ResourceAttrInfo *ai, double value);
	virtual void SetAttrString(const char *name, const std::string &value);
	virtual void SetAttrString(ResourceAttrInfo *ai, const std::string &value);
	virtual void SetAttrIpAddr(const char *name, IpAddr &value);
	virtual void SetAttrIpAddr(ResourceAttrInfo *ai, IpAddr &value);
	virtual void SetAttrArrayInt32(const char *name, int index, int32_t value);
	virtual void SetAttrArrayInt32(ResourceAttrInfo *ai, int index, int32_t value);
	virtual void SetAttrArrayUInt32(const char *name, int index, uint32_t value);
	virtual void SetAttrArrayUInt32(ResourceAttrInfo *ai, int index, uint32_t value);
	virtual void SetAttrArrayInt64(const char *name, int index, int64_t value);
	virtual void SetAttrArrayInt64(ResourceAttrInfo *ai, int index, int64_t value);
	virtual void SetAttrArrayUInt64(const char *name, int index, uint64_t value);
	virtual void SetAttrArrayUInt64(ResourceAttrInfo *ai, int index, uint64_t value);
	virtual void SetAttrArrayDouble(const char *name, int index, double value);
	virtual void SetAttrArrayDouble(ResourceAttrInfo *ai, int index, double value);
	virtual void SetAttrArrayString(const char *name, int index, const std::string &value);
	virtual void SetAttrArrayString(ResourceAttrInfo *ai, int index, const std::string &value);
	virtual void SetAttrArrayIpAddr(const char *name, int index, IpAddr &value);
	virtual void SetAttrArrayIpAddr(ResourceAttrInfo *ai, int index, IpAddr &value);
	virtual void SetAttrHashInt32(const char *name, const std::string &index, int32_t value);
	virtual void SetAttrHashInt32(ResourceAttrInfo *ai, const std::string &index, int32_t value);
	virtual void SetAttrHashUInt32(const char *name, const std::string &index, uint32_t value);
	virtual void SetAttrHashUInt32(ResourceAttrInfo *ai, const std::string &index, uint32_t value);
	virtual void SetAttrHashInt64(const char *name, const std::string &index, int64_t value);
	virtual void SetAttrHashInt64(ResourceAttrInfo *ai, const std::string &index, int64_t value);
	virtual void SetAttrHashUInt64(const char *name, const std::string &index, uint64_t value);
	virtual void SetAttrHashUInt64(ResourceAttrInfo *ai, const std::string &index, uint64_t value);
	virtual void SetAttrHashDouble(const char *name, const std::string &index, double value);
	virtual void SetAttrHashDouble(ResourceAttrInfo *ai, const std::string &index, double value);
	virtual void SetAttrHashString(const char *name, const std::string &index, const std::string &value);
	virtual void SetAttrHashString(ResourceAttrInfo *ai, const std::string &index, const std::string &value);
	virtual void SetAttrHashIpAddr(const char *name, const std::string &index, IpAddr &value);
	virtual void SetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index, IpAddr &value);

	virtual void Clear(ResourceAttrInfo *ai);
	virtual void Clear(const char *name);
	virtual void DeleteHashItem(ResourceAttrInfo *ai, const std::string&);
	virtual void DeleteHashItem(const char *name, const std::string&);
	virtual int GetCount(ResourceAttrInfo *ai);
	virtual int GetCount(const char *name);
	virtual std::vector<std::string> *GetKeys(ResourceAttrInfo *ai);
	virtual std::vector<std::string> *GetKeys(const char *name);
	virtual std::vector<int32_t> *GetValuesInt32(ResourceAttrInfo *ai);
	virtual std::vector<int32_t> *GetValuesInt32(const char *name);
	virtual std::vector<uint32_t> *GetValuesUInt32(ResourceAttrInfo *ai);
	virtual std::vector<uint32_t> *GetValuesUInt32(const char *name);
	virtual std::vector<int64_t> *GetValuesInt64(ResourceAttrInfo *ai);
	virtual std::vector<int64_t> *GetValuesInt64(const char *name);
	virtual std::vector<uint64_t> *GetValuesUInt64(ResourceAttrInfo *ai);
	virtual std::vector<uint64_t> *GetValuesUInt64(const char *name);
	virtual std::vector<double> *GetValuesDouble(ResourceAttrInfo *ai);
	virtual std::vector<double> *GetValuesDouble(const char *name);
	virtual std::vector<std::string> *GetValuesString(ResourceAttrInfo *ai);
	virtual std::vector<std::string> *GetValuesString(const char *name);
	virtual std::vector<IpAddr> *GetValuesIpAddr(ResourceAttrInfo *ai);
	virtual std::vector<IpAddr> *GetValuesIpAddr(const char *name);
	
	// flags (only used in Processor)
	void SetFlag(Flag flag);
	void ResetFlag(Flag flag);
	bool IsSetFlag(Flag flag);

	// resource may contain link to other resource, it is only kept only in the memory
	Resource *GetAttachedResource();
	void SetAttachedResource(Resource *attachedResource);
	void ClearAttachedResource();

	// Following methods are helper methods to be used by other objects
	// (basically wrappers around GetResourceInfo)

	// type id of a resource (to be used by Resources::AcquireResource(typeid))
	int GetTypeId();
	// type string of a resource
	const char *GetTypeString(bool terse = false);
	// object name (for construction of an object or a reference)
	const char *GetObjectName();
	// return ResourceAttrInfo describing one field
	std::vector<ResourceAttrInfo*> *GetAttrInfoList();
	// return terse resource info, intended for logging
	std::string ToStringShort();

	// Static helpers (Resource registry and Serialization/Deserialization)

	// resource registry, all resources should be registered there
	static void CreateRegistry();
	static void DeleteRegistry();
	static ResourceRegistry *GetRegistry();

	// serializes resource (together with size and type), returns total bytes written
	static bool SerializeResource(Resource *resource, ResourceOutputStream &stream, bool saveType = false, bool saveIdStatus = false);
	static Resource *DeserializeResource(ResourceInputStream &stream, int resourceType, int *totalSize);

protected:
	// following resource properties are usually memory-only
	// id of the resource, should be unique in run-time
	// N.B.: id should never be overwritten in Deserialize()
	int id;
	// status that can be used for filtering in PE filters
	int status;
	// only used in Processor
	int flags;
	// resource attached to this one
	Resource *attachedResource;

	// helper, so that we can return empty string reference
	static std::string empty;

	// resource registry, all resources should be registered there
	static ResourceRegistry *registry;

	static log4cxx::LoggerPtr logger;
};

inline ResourceInfo::~ResourceInfo() {
	if (attrInfoList) {
		for (std::vector<ResourceAttrInfo*>::iterator iter = attrInfoList->begin(); iter != attrInfoList->end(); ++iter)
			delete *iter;
		delete attrInfoList;
	}
}

inline int ResourceInfo::GetTypeId() {
	return typeId;
}

inline void ResourceInfo::SetTypeId(int typeId) {
	this->typeId = typeId;
}

inline const char *ResourceInfo::GetTypeString() {
	return typeString;
}

inline void ResourceInfo::SetTypeString(const char *typeString) {
	free(this->typeString);
	this->typeString = strdup(typeString);
}

inline const char *ResourceInfo::GetTypeStringTerse() {
	return typeStringTerse;
}

inline void ResourceInfo::SetTypeStringTerse(const char *typeStringTerse) {
	free(this->typeStringTerse);
	this->typeStringTerse = strdup(typeStringTerse);
}

inline const char *ResourceInfo::GetObjectName() {
	return objectName;
}

inline void ResourceInfo::SetObjectName(const char *objectName) {
	free(this->objectName);
	this->objectName = strdup(objectName);
}

inline std::vector<ResourceAttrInfo*> *ResourceInfo::GetAttrInfoList() {
	return attrInfoList;
}

inline void ResourceInfo::SetAttrInfoList(std::vector<ResourceAttrInfo*> *attrInfoList) {
	if (this->attrInfoList) {
		for (std::vector<ResourceAttrInfo*>::iterator iter = this->attrInfoList->begin(); iter != this->attrInfoList->end(); ++iter)
			delete *iter;
		delete this->attrInfoList;
	}
	this->attrInfoList = attrInfoList;
}

inline Resource::Resource() {
	flags = 0;
	SetId(registry->NextResourceId());
	status = 0;
	attachedResource = NULL;
}

inline Resource::Resource(const Resource &r) : status(r.status), flags(r.flags), attachedResource(r.attachedResource) {
	SetId(registry->NextResourceId());
}

inline void Resource::Clear() {
	flags = 0;
	status = 0;
	id = 0;
	attachedResource = NULL;
}

inline int Resource::GetId() {
	return id;
}

inline void Resource::SetId(int id) {
	this->id = id;
}

inline int Resource::GetStatus() {
	return status;
}

inline void Resource::SetStatus(int status) {
	this->status = status;
}

inline void Resource::SetFlag(Flag flag) {
	this->flags |= flag;
}

inline void Resource::ResetFlag(Flag flag) {
	this->flags &= 0xFFFF ^ flag;
}

inline bool Resource::IsSetFlag(Flag flag) {
	return (this->flags & flag) != 0;
}

inline void Resource::SetAttachedResource(Resource *attachedResource) {
	this->attachedResource = attachedResource;
}

inline Resource *Resource::GetAttachedResource() {
	return attachedResource;
}

inline void Resource::ClearAttachedResource() {
	attachedResource = NULL;
}

inline int Resource::GetTypeId() {
	return GetResourceInfo()->GetTypeId();
}

inline const char *Resource::GetTypeString(bool terse) {
	ResourceInfo *ri = GetResourceInfo();
	return terse ? ri->GetTypeStringTerse() : ri->GetTypeString();
}

inline const char *Resource::GetObjectName() {
	return GetResourceInfo()->GetObjectName();
}

inline std::vector<ResourceAttrInfo*> *Resource::GetAttrInfoList() {
	return GetResourceInfo()->GetAttrInfoList();
}

inline std::string Resource::ToStringShort() {
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "[%s %d %d]", GetTypeString(true), GetId(), IsSetFlag(DELETED) ? -1 : GetStatus());
	return buffer;
}

inline int32_t Resource::GetAttrInt32(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetInt32(this);
}

inline int32_t Resource::GetAttrInt32(ResourceAttrInfo *ai) {
	return ai->GetInt32(this);
}

inline uint32_t Resource::GetAttrUInt32(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetUInt32(this);
}

inline uint32_t Resource::GetAttrUInt32(ResourceAttrInfo *ai) {
	return ai->GetUInt32(this);
}

inline int64_t Resource::GetAttrInt64(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetInt64(this);
}

inline int64_t Resource::GetAttrInt64(ResourceAttrInfo *ai) {
	return ai->GetInt64(this);
}

inline uint64_t Resource::GetAttrUInt64(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetUInt64(this);
}

inline uint64_t Resource::GetAttrUInt64(ResourceAttrInfo *ai) {
	return ai->GetUInt64(this);
}

inline double Resource::GetAttrDouble(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetDouble(this);
}

inline double Resource::GetAttrDouble(ResourceAttrInfo *ai) {
	return ai->GetDouble(this);
}

inline const std::string Resource::GetAttrString(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetString(this) : empty;
}

inline const std::string Resource::GetAttrString(ResourceAttrInfo *ai) {
	return ai->GetString(this);
}

inline const IpAddr Resource::GetAttrIpAddr(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetIpAddr(this);
}

inline const IpAddr Resource::GetAttrIpAddr(ResourceAttrInfo *ai) {
	return ai->GetIpAddr(this);
}

inline int32_t Resource::GetAttrArrayInt32(const char *name, int index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetArrayInt32(this, index);
}

inline int32_t Resource::GetAttrArrayInt32(ResourceAttrInfo *ai, int index) {
	return ai->GetArrayInt32(this, index);
}

inline uint32_t Resource::GetAttrArrayUInt32(const char *name, int index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetArrayUInt32(this, index);
}

inline uint32_t Resource::GetAttrArrayUInt32(ResourceAttrInfo *ai, int index) {
	return ai->GetArrayUInt32(this, index);
}

inline int64_t Resource::GetAttrArrayInt64(const char *name, int index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetArrayInt64(this, index);
}

inline int64_t Resource::GetAttrArrayInt64(ResourceAttrInfo *ai, int index) {
	return ai->GetArrayInt64(this, index);
}

inline uint64_t Resource::GetAttrArrayUInt64(const char *name, int index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetArrayUInt64(this, index);
}

inline uint64_t Resource::GetAttrArrayUInt64(ResourceAttrInfo *ai, int index) {
	return ai->GetArrayUInt64(this, index);
}

inline double Resource::GetAttrArrayDouble(const char *name, int index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetArrayDouble(this, index);
}

inline double Resource::GetAttrArrayDouble(ResourceAttrInfo *ai, int index) {
	return ai->GetArrayDouble(this, index);
}

inline const std::string Resource::GetAttrArrayString(const char *name, int index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetArrayString(this, index);
}

inline const std::string Resource::GetAttrArrayString(ResourceAttrInfo *ai, int index) {
	return ai->GetArrayString(this, index);
}

inline const IpAddr Resource::GetAttrArrayIpAddr(const char *name, int index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetArrayIpAddr(this, index);
}

inline const IpAddr Resource::GetAttrArrayIpAddr(ResourceAttrInfo *ai, int index) {
	return ai->GetArrayIpAddr(this, index);
}

inline int32_t Resource::GetAttrHashInt32(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetHashInt32(this, index);
}

inline int32_t Resource::GetAttrHashInt32(ResourceAttrInfo *ai, const std::string &index) {
	return ai->GetHashInt32(this, index);
}

inline uint32_t Resource::GetAttrHashUInt32(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetHashUInt32(this, index);
}

inline uint32_t Resource::GetAttrHashUInt32(ResourceAttrInfo *ai, const std::string &index) {
	return ai->GetHashUInt32(this, index);
}

inline int64_t Resource::GetAttrHashInt64(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetHashInt64(this, index);
}

inline int64_t Resource::GetAttrHashInt64(ResourceAttrInfo *ai, const std::string &index) {
	return ai->GetHashInt64(this, index);
}

inline uint64_t Resource::GetAttrHashUInt64(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetHashUInt64(this, index);
}

inline uint64_t Resource::GetAttrHashUInt64(ResourceAttrInfo *ai, const std::string &index) {
	return ai->GetHashUInt64(this, index);
}

inline double Resource::GetAttrHashDouble(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetHashDouble(this, index);
}

inline double Resource::GetAttrHashDouble(ResourceAttrInfo *ai, const std::string &index) {
	return ai->GetHashDouble(this, index);
}

inline const std::string Resource::GetAttrHashString(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetHashString(this, index);
}

inline const std::string Resource::GetAttrHashString(ResourceAttrInfo *ai, const std::string &index) {
	return ai->GetHashString(this, index);
}

inline const IpAddr Resource::GetAttrHashIpAddr(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetHashIpAddr(this, index);
}

inline const IpAddr Resource::GetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index) {
	return ai->GetHashIpAddr(this, index);
}

inline void Resource::SetAttrInt32(const char *name, int32_t value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetInt32(this, value);
}

inline void Resource::SetAttrInt32(ResourceAttrInfo *ai, int32_t value) {
	ai->SetInt32(this, value);
}

inline void Resource::SetAttrUInt32(const char *name, uint32_t value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetUInt32(this, value);
}

inline void Resource::SetAttrUInt32(ResourceAttrInfo *ai, uint32_t value) {
	ai->SetUInt32(this, value);
}

inline void Resource::SetAttrInt64(const char *name, int64_t value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetInt64(this, value);
}

inline void Resource::SetAttrInt64(ResourceAttrInfo *ai, int64_t value) {
	ai->SetInt64(this, value);
}

inline void Resource::SetAttrUInt64(const char *name, uint64_t value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetUInt64(this, value);
}

inline void Resource::SetAttrUInt64(ResourceAttrInfo *ai, uint64_t value) {
	ai->SetUInt64(this, value);
}

inline void Resource::SetAttrDouble(const char *name, double value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetDouble(this, value);
}

inline void Resource::SetAttrDouble(ResourceAttrInfo *ai, double value) {
	ai->SetDouble(this, value);
}

inline void Resource::SetAttrString(const char *name, const std::string &value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetString(this, value);
}

inline void Resource::SetAttrString(ResourceAttrInfo *ai, const std::string &value) {
	ai->SetString(this, value);
}

inline void Resource::SetAttrIpAddr(const char *name, IpAddr &value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetIpAddr(this, value);
}

inline void Resource::SetAttrIpAddr(ResourceAttrInfo *ai, IpAddr &value) {
	ai->SetIpAddr(this, value);
}

inline void Resource::SetAttrArrayInt32(const char *name, int index, int32_t value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetArrayInt32(this, index, value);
}

inline void Resource::SetAttrArrayInt32(ResourceAttrInfo *ai, int index, int32_t value) {
	ai->SetArrayInt32(this, index, value);
}

inline void Resource::SetAttrArrayUInt32(const char *name, int index, uint32_t value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetArrayUInt32(this, index, value);
}

inline void Resource::SetAttrArrayUInt32(ResourceAttrInfo *ai, int index, uint32_t value) {
	ai->SetArrayUInt32(this, index, value);
}

inline void Resource::SetAttrArrayInt64(const char *name, int index, int64_t value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetArrayInt64(this, index, value);
}

inline void Resource::SetAttrArrayInt64(ResourceAttrInfo *ai, int index, int64_t value) {
	ai->SetArrayInt64(this, index, value);
}

inline void Resource::SetAttrArrayUInt64(const char *name, int index, uint64_t value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetArrayUInt64(this, index, value);
}

inline void Resource::SetAttrArrayUInt64(ResourceAttrInfo *ai, int index, uint64_t value) {
	ai->SetArrayUInt64(this, index, value);
}

inline void Resource::SetAttrArrayDouble(const char *name, int index, double value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetArrayDouble(this, index, value);
}

inline void Resource::SetAttrArrayDouble(ResourceAttrInfo *ai, int index, double value) {
	ai->SetArrayDouble(this, index, value);
}

inline void Resource::SetAttrArrayString(const char *name, int index, const std::string &value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetArrayString(this, index, value);
}

inline void Resource::SetAttrArrayString(ResourceAttrInfo *ai, int index, const std::string &value) {
	ai->SetArrayString(this, index, value);
}

inline void Resource::SetAttrArrayIpAddr(const char *name, int index, IpAddr &value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetArrayIpAddr(this, index, value);
}

inline void Resource::SetAttrArrayIpAddr(ResourceAttrInfo *ai, int index, IpAddr &value) {
	ai->SetArrayIpAddr(this, index, value);
}

inline void Resource::SetAttrHashInt32(const char *name, const std::string &index, int32_t value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetHashInt32(this, index, value);
}

inline void Resource::SetAttrHashInt32(ResourceAttrInfo *ai, const std::string &index, int32_t value) {
	ai->SetHashInt32(this, index, value);
}

inline void Resource::SetAttrHashUInt32(const char *name, const std::string &index, uint32_t value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetHashUInt32(this, index, value);
}

inline void Resource::SetAttrHashUInt32(ResourceAttrInfo *ai, const std::string &index, uint32_t value) {
	ai->SetHashUInt32(this, index, value);
}

inline void Resource::SetAttrHashInt64(const char *name, const std::string &index, int64_t value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetHashInt64(this, index, value);
}

inline void Resource::SetAttrHashInt64(ResourceAttrInfo *ai, const std::string &index, int64_t value) {
	ai->SetHashInt64(this, index, value);
}

inline void Resource::SetAttrHashUInt64(const char *name, const std::string &index, uint64_t value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetHashUInt64(this, index, value);
}

inline void Resource::SetAttrHashUInt64(ResourceAttrInfo *ai, const std::string &index, uint64_t value) {
	ai->SetHashUInt64(this, index, value);
}

inline void Resource::SetAttrHashDouble(const char *name, const std::string &index, double value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetHashDouble(this, index, value);
}

inline void Resource::SetAttrHashDouble(ResourceAttrInfo *ai, const std::string &index, double value) {
	ai->SetHashDouble(this, index, value);
}

inline void Resource::SetAttrHashString(const char *name, const std::string &index, const std::string &value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetHashString(this, index, value);
}

inline void Resource::SetAttrHashString(ResourceAttrInfo *ai, const std::string &index, const std::string &value) {
	ai->SetHashString(this, index, value);
}

inline void Resource::SetAttrHashIpAddr(const char *name, const std::string &index, IpAddr &value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetHashIpAddr(this, index, value);
}

inline void Resource::SetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index, IpAddr &value) {
	ai->SetHashIpAddr(this, index, value);
}

inline void Resource::Clear(ResourceAttrInfo *ai) {
	ai->Clear(this);
}

inline void Resource::Clear(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->Clear(this);
}

inline void Resource::DeleteHashItem(ResourceAttrInfo *ai, const std::string &name) {
	ai->DeleteHashItem(this, name);
}

inline void Resource::DeleteHashItem(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->DeleteHashItem(this, name);
}

inline int Resource::GetCount(ResourceAttrInfo *ai) {
	return ai->GetCount(this);
}

inline int Resource::GetCount(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetCount(this) : -1;
}

inline std::vector<std::string> *Resource::GetKeys(ResourceAttrInfo *ai) {
	return ai->GetKeys(this);
}

inline std::vector<std::string> *Resource::GetKeys(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetKeys(this) : NULL;
}

inline std::vector<int32_t> *Resource::GetValuesInt32(ResourceAttrInfo *ai) {
	return ai->GetValuesInt32(this);
}

inline std::vector<int32_t> *Resource::GetValuesInt32(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetValuesInt32(this) : NULL;
}

inline std::vector<uint32_t> *Resource::GetValuesUInt32(ResourceAttrInfo *ai) {
	return ai->GetValuesUInt32(this);
}

inline std::vector<uint32_t> *Resource::GetValuesUInt32(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetValuesUInt32(this) : NULL;
}

inline std::vector<int64_t> *Resource::GetValuesInt64(ResourceAttrInfo *ai) {
	return ai->GetValuesInt64(this);
}

inline std::vector<int64_t> *Resource::GetValuesInt64(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetValuesInt64(this) : NULL;
}

inline std::vector<uint64_t> *Resource::GetValuesUInt64(ResourceAttrInfo *ai) {
	return ai->GetValuesUInt64(this);
}

inline std::vector<uint64_t> *Resource::GetValuesUInt64(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetValuesUInt64(this) : NULL;
}

inline std::vector<double> *Resource::GetValuesDouble(ResourceAttrInfo *ai) {
	return ai->GetValuesDouble(this);
}

inline std::vector<double> *Resource::GetValuesDouble(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetValuesDouble(this) : NULL;
}

inline std::vector<std::string> *Resource::GetValuesString(ResourceAttrInfo *ai) {
	return ai->GetValuesString(this);
}

inline std::vector<std::string> *Resource::GetValuesString(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetValuesString(this) : NULL;
}

inline std::vector<IpAddr> *Resource::GetValuesIpAddr(ResourceAttrInfo *ai) {
	return ai->GetValuesIpAddr(this);
}

inline std::vector<IpAddr> *Resource::GetValuesIpAddr(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetValuesIpAddr(this) : NULL;
}

inline void Resource::CreateRegistry() {
	if (!registry)
		registry = new ResourceRegistry();
}

inline void Resource::DeleteRegistry() {
	delete registry;
	registry = NULL;
}

inline ResourceRegistry *Resource::GetRegistry() {
	return registry;
}

// logger helper macros (print short info about the resource)

#define LOG4CXX_TRACE_R(logger, r, ...) { LOG4CXX_TRACE(logger, "[" << r->GetTypeString(true) << " " << r->GetId() << " " << r->GetStatus() << (r->IsSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG4CXX_DEBUG_R(logger, r, ...) { LOG4CXX_DEBUG(logger, "[" << r->GetTypeString(true) << " " << r->GetId() << " " << r->GetStatus() << (r->IsSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG4CXX_INFO_R(logger, r, ...) { LOG4CXX_INFO(logger, "[" << r->GetTypeString(true) << " " << r->GetId() << " " << r->GetStatus() << (r->IsSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG4CXX_ERROR_R(logger, r, ...) { LOG4CXX_ERROR(logger, "[" << r->GetTypeString(true) << " " << r->GetId() << " " << r->GetStatus() << (r->IsSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG4CXX_FATAL_R(logger, r, ...) { LOG4CXX_FATAL(logger, "[" << r->GetTypeString(true) << " " << r->GetId() << " " << r->GetStatus() << (r->IsSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }

#define LOG_TRACE_R(o, r, ...) { LOG4CXX_TRACE(o->getLogger(), o->GetId() << ": [" << r->GetTypeString(true) << " " << r->GetId() << " " << r->GetStatus() << (r->IsSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG_DEBUG_R(o, r, ...) { LOG4CXX_DEBUG(o->getLogger(), o->GetId() << ": [" << r->GetTypeString(true) << " " << r->GetId() << " " << r->GetStatus() << (r->IsSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG_INFO_R(o, r, ...) { LOG4CXX_INFO(o->getLogger(), o->GetId() << ": [" << r->GetTypeString(true) << " " << r->GetId() << " " << r->GetStatus() << (r->IsSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG_ERROR_R(o, r, ...) { LOG4CXX_ERROR(o->getLogger(), o->GetId() << ": [" << r->GetTypeString(true) << " " << r->GetId() << " " << r->GetStatus() << (r->IsSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG_FATAL_R(o, r, ...) { LOG4CXX_FATAL(o->getLogger(), o->GetId() << ": [" << r->GetTypeString(true) << " " << r->GetId() << " " << r->GetStatus() << (r->IsSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }

#endif
