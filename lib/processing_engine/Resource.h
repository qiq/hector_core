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

class ResourceAttrInfo;
class ResourceInfo;
namespace google {
	namespace protobuf {
		namespace io {
	class CodedInputStream;
	class CodedOutputStream;
		}
	}
}

class Resource {
public:
	enum Flag {
		DELETED = 1,		// resource will be deleted
		SKIP = 2,		// the rest of processor modules should be skipped, resource is just appended to the queue
	};
	
	Resource();
	Resource(const Resource &r);
	virtual ~Resource() {};
	// Create copy of a resource
	virtual Resource *Clone() = 0;
	// Clear current resource (as delete + new would do, except id is set to 0)
	virtual void Clear();
	// Returns true if resource is serialized using Protocol Buffers. We
	// cannot rely on the dynamic_cast as we use shared libraries and dlopen.
	virtual bool IsProtobufResource();
	// save and restore resource
	virtual std::string *Serialize() = 0;
	// data need not be nul-terminated
	virtual bool Deserialize(const char *data, int size) = 0;
	// get info about a resource field
	virtual std::vector<ResourceAttrInfo*> *GetAttrInfoList() = 0;
	// type id of a resource (to be used by Resources::AcquireResource(typeid))
	virtual int GetTypeId() = 0;
	// type string of a resource
	virtual const char *GetTypeString(bool terse = false) = 0;
	// used by queues in case there is limit on queue size, this size may
	// be somewhat arbitrary
	virtual int GetSize() = 0;
	// return string representation of the resource (e.g. for debugging purposes)
	virtual std::string ToString(Object::LogLevel = Object::INFO) = 0;
	// id should be unique across all in-memory resources
	virtual int GetId();
	virtual void SetId(int id);
	// status may be tested in Processor to select target queue
	virtual int GetStatus();
	virtual void SetStatus(int status);

	// generic API for getting/setting of attributes
	virtual const std::string GetAttrString(const char *name);
	virtual const std::string GetAttrString(ResourceAttrInfo *ai);
	virtual int GetAttrInt(const char *name);
	virtual int GetAttrInt(ResourceAttrInfo *ai);
	virtual long GetAttrLong(const char *name);
	virtual long GetAttrLong(ResourceAttrInfo *ai);
	virtual const IpAddr GetAttrIpAddr(const char *name);
	virtual const IpAddr GetAttrIpAddr(ResourceAttrInfo *ai);
	virtual const std::string GetAttrArrayString(const char *name, int index);
	virtual const std::string GetAttrArrayString(ResourceAttrInfo *ai, int index);
	virtual int GetAttrArrayInt(const char *name, int index);
	virtual int GetAttrArrayInt(ResourceAttrInfo *ai, int index);
	virtual long GetAttrArrayLong(const char *name, int index);
	virtual long GetAttrArrayLong(ResourceAttrInfo *ai, int index);
	virtual const IpAddr GetAttrArrayIpAddr(const char *name, int index);
	virtual const IpAddr GetAttrArrayIpAddr(ResourceAttrInfo *ai, int index);
	virtual const std::string GetAttrHashString(const char *name, const std::string &index);
	virtual const std::string GetAttrHashString(ResourceAttrInfo *ai, const std::string &index);
	virtual int GetAttrHashInt(const char *name, const std::string &index);
	virtual int GetAttrHashInt(ResourceAttrInfo *ai, const std::string &index);
	virtual long GetAttrHashLong(const char *name, const std::string &index);
	virtual long GetAttrHashLong(ResourceAttrInfo *ai, const std::string &index);
	virtual const IpAddr GetAttrHashIpAddr(const char *name, const std::string &index);
	virtual const IpAddr GetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index);

	virtual void SetAttrString(const char *name, const std::string &value);
	virtual void SetAttrString(ResourceAttrInfo *ai, const std::string &value);
	virtual void SetAttrInt(const char *name, int value);
	virtual void SetAttrInt(ResourceAttrInfo *ai, int value);
	virtual void SetAttrLong(const char *name, long value);
	virtual void SetAttrLong(ResourceAttrInfo *ai, long value);
	virtual void SetAttrIpAddr(const char *name, IpAddr &value);
	virtual void SetAttrIpAddr(ResourceAttrInfo *ai, IpAddr &value);
	virtual void SetAttrArrayString(const char *name, int index, const std::string &value);
	virtual void SetAttrArrayString(ResourceAttrInfo *ai, int index, const std::string &value);
	virtual void SetAttrArrayInt(const char *name, int index, int value);
	virtual void SetAttrArrayInt(ResourceAttrInfo *ai, int index, int value);
	virtual void SetAttrArrayLong(const char *name, int index, long value);
	virtual void SetAttrArrayLong(ResourceAttrInfo *ai, int index, long value);
	virtual void SetAttrArrayIpAddr(const char *name, int index, IpAddr &value);
	virtual void SetAttrArrayIpAddr(ResourceAttrInfo *ai, int index, IpAddr &value);
	virtual void SetAttrHashString(const char *name, const std::string &index, const std::string &value);
	virtual void SetAttrHashString(ResourceAttrInfo *ai, const std::string &index, const std::string &value);
	virtual void SetAttrHashInt(const char *name, const std::string &index, int value);
	virtual void SetAttrHashInt(ResourceAttrInfo *ai, const std::string &index, int value);
	virtual void SetAttrHashLong(const char *name, const std::string &index, long value);
	virtual void SetAttrHashLong(ResourceAttrInfo *ai, const std::string &index, long value);
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
	virtual std::vector<std::string> *GetValuesString(ResourceAttrInfo *ai);
	virtual std::vector<std::string> *GetValuesString(const char *name);
	virtual std::vector<int> *GetValuesInt(ResourceAttrInfo *ai);
	virtual std::vector<int> *GetValuesInt(const char *name);
	virtual std::vector<long> *GetValuesLong(ResourceAttrInfo *ai);
	virtual std::vector<long> *GetValuesLong(const char *name);
	virtual std::vector<IpAddr> *GetValuesIpAddr(ResourceAttrInfo *ai);
	virtual std::vector<IpAddr> *GetValuesIpAddr(const char *name);
	
	// flags 
	void SetFlag(Flag flag);
	void ResetFlag(Flag flag);
	bool IsSetFlag(Flag flag);

	// resource may contain link to other resource, it is only kept only in the memory
	Resource *GetAttachedResource();
	void SetAttachedResource(Resource *attachedResource);
	void ClearAttachedResource();

	// return terse resource info, intended for logging
	std::string ToStringShort();

	// resource registry, all resources should be registered there
	static ResourceRegistry *registry;

	// serializes resource (together with size and type), returns total bytes written
	static bool Serialize(Resource *resource, google::protobuf::io::CodedOutputStream *stream);
	static Resource *Deserialize(google::protobuf::io::CodedInputStream *stream, int *totalSize);

protected:
	// memory-only, used just in Processor
	int flags;
	// these are memory-only, some resources may decide to keep them
	// on-disk too
	// N.B.: id should not be overwritten in Deserialize()
	int id;
	int status;
	Resource *attachedResource;

	// helper, so that we can return empty string reference
	static std::string empty;

	static log4cxx::LoggerPtr logger;
};

inline bool Resource::IsProtobufResource() {
	return false;
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

inline std::string Resource::ToStringShort() {
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "[%s %d %d]", GetTypeString(true), GetId(), IsSetFlag(DELETED) ? -1 : GetStatus());
	return buffer;
}

inline const std::string Resource::GetAttrString(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetString(this) : empty;
}

inline const std::string Resource::GetAttrString(ResourceAttrInfo *ai) {
	return ai->GetString(this);
}

inline int Resource::GetAttrInt(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetInt(this);
}

inline int Resource::GetAttrInt(ResourceAttrInfo *ai) {
	return ai->GetInt(this);
}

inline long Resource::GetAttrLong(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetLong(this);
}

inline long Resource::GetAttrLong(ResourceAttrInfo *ai) {
	return ai->GetLong(this);
}

inline const IpAddr Resource::GetAttrIpAddr(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetIpAddr(this);
}

inline const IpAddr Resource::GetAttrIpAddr(ResourceAttrInfo *ai) {
	return ai->GetIpAddr(this);
}

inline const std::string Resource::GetAttrArrayString(const char *name, int index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetArrayString(this, index);
}

inline const std::string Resource::GetAttrArrayString(ResourceAttrInfo *ai, int index) {
	return ai->GetArrayString(this, index);
}

inline int Resource::GetAttrArrayInt(const char *name, int index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetArrayInt(this, index);
}

inline int Resource::GetAttrArrayInt(ResourceAttrInfo *ai, int index) {
	return ai->GetArrayInt(this, index);
}

inline long Resource::GetAttrArrayLong(const char *name, int index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetArrayLong(this, index);
}

inline long Resource::GetAttrArrayLong(ResourceAttrInfo *ai, int index) {
	return ai->GetArrayLong(this, index);
}

inline const IpAddr Resource::GetAttrArrayIpAddr(const char *name, int index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetArrayIpAddr(this, index);
}

inline const IpAddr Resource::GetAttrArrayIpAddr(ResourceAttrInfo *ai, int index) {
	return ai->GetArrayIpAddr(this, index);
}

inline const std::string Resource::GetAttrHashString(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetHashString(this, index);
}

inline const std::string Resource::GetAttrHashString(ResourceAttrInfo *ai, const std::string &index) {
	return ai->GetHashString(this, index);
}

inline int Resource::GetAttrHashInt(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetHashInt(this, index);
}

inline int Resource::GetAttrHashInt(ResourceAttrInfo *ai, const std::string &index) {
	return ai->GetHashInt(this, index);
}

inline long Resource::GetAttrHashLong(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetHashLong(this, index);
}

inline long Resource::GetAttrHashLong(ResourceAttrInfo *ai, const std::string &index) {
	return ai->GetHashLong(this, index);
}

inline const IpAddr Resource::GetAttrHashIpAddr(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai->GetHashIpAddr(this, index);
}

inline const IpAddr Resource::GetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index) {
	return ai->GetHashIpAddr(this, index);
}

inline void Resource::SetAttrString(const char *name, const std::string &value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetString(this, value);
}

inline void Resource::SetAttrString(ResourceAttrInfo *ai, const std::string &value) {
	ai->SetString(this, value);
}

inline void Resource::SetAttrInt(const char *name, int value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetInt(this, value);
}

inline void Resource::SetAttrInt(ResourceAttrInfo *ai, int value) {
	ai->SetInt(this, value);
}

inline void Resource::SetAttrLong(const char *name, long value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetLong(this, value);
}

inline void Resource::SetAttrLong(ResourceAttrInfo *ai, long value) {
	ai->SetLong(this, value);
}

inline void Resource::SetAttrIpAddr(const char *name, IpAddr &value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetIpAddr(this, value);
}

inline void Resource::SetAttrIpAddr(ResourceAttrInfo *ai, IpAddr &value) {
	ai->SetIpAddr(this, value);
}

inline void Resource::SetAttrArrayString(const char *name, int index, const std::string &value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetArrayString(this, index, value);
}

inline void Resource::SetAttrArrayString(ResourceAttrInfo *ai, int index, const std::string &value) {
	ai->SetArrayString(this, index, value);
}

inline void Resource::SetAttrArrayInt(const char *name, int index, int value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetArrayInt(this, index, value);
}

inline void Resource::SetAttrArrayInt(ResourceAttrInfo *ai, int index, int value) {
	ai->SetArrayInt(this, index, value);
}

inline void Resource::SetAttrArrayLong(const char *name, int index, long value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetArrayLong(this, index, value);
}

inline void Resource::SetAttrArrayLong(ResourceAttrInfo *ai, int index, long value) {
	ai->SetArrayLong(this, index, value);
}

inline void Resource::SetAttrArrayIpAddr(const char *name, int index, IpAddr &value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetArrayIpAddr(this, index, value);
}

inline void Resource::SetAttrArrayIpAddr(ResourceAttrInfo *ai, int index, IpAddr &value) {
	ai->SetArrayIpAddr(this, index, value);
}

inline void Resource::SetAttrHashString(const char *name, const std::string &index, const std::string &value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetHashString(this, index, value);
}

inline void Resource::SetAttrHashString(ResourceAttrInfo *ai, const std::string &index, const std::string &value) {
	ai->SetHashString(this, index, value);
}

inline void Resource::SetAttrHashInt(const char *name, const std::string &index, int value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetHashInt(this, index, value);
}

inline void Resource::SetAttrHashInt(ResourceAttrInfo *ai, const std::string &index, int value) {
	ai->SetHashInt(this, index, value);
}

inline void Resource::SetAttrHashLong(const char *name, const std::string &index, long value) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	if (ai)
		ai->SetHashLong(this, index, value);
}

inline void Resource::SetAttrHashLong(ResourceAttrInfo *ai, const std::string &index, long value) {
	ai->SetHashLong(this, index, value);
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

inline std::vector<std::string> *Resource::GetValuesString(ResourceAttrInfo *ai) {
	return ai->GetValuesString(this);
}

inline std::vector<std::string> *Resource::GetValuesString(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetValuesString(this) : NULL;
}

inline std::vector<int> *Resource::GetValuesInt(ResourceAttrInfo *ai) {
	return ai->GetValuesInt(this);
}

inline std::vector<int> *Resource::GetValuesInt(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetValuesInt(this) : NULL;
}

inline std::vector<long> *Resource::GetValuesLong(ResourceAttrInfo *ai) {
	return ai->GetValuesLong(this);
}

inline std::vector<long> *Resource::GetValuesLong(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetValuesLong(this) : NULL;
}

inline std::vector<IpAddr> *Resource::GetValuesIpAddr(ResourceAttrInfo *ai) {
	return ai->GetValuesIpAddr(this);
}

inline std::vector<IpAddr> *Resource::GetValuesIpAddr(const char *name) {
	ResourceAttrInfo *ai = registry->GetAttrInfo(GetTypeId(), name);
	return ai ? ai->GetValuesIpAddr(this) : NULL;
}

#endif
