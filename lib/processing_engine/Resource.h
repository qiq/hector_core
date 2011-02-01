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

#define LOG4CXX_TRACE_R(logger, r, ...) { LOG4CXX_TRACE(logger, "[" << r->getTypeStrShort() << " " << r->getId() << " " << r->getStatus() << (r->isSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG4CXX_DEBUG_R(logger, r, ...) { LOG4CXX_DEBUG(logger, "[" << r->getTypeStrShort() << " " << r->getId() << " " << r->getStatus() << (r->isSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG4CXX_INFO_R(logger, r, ...) { LOG4CXX_INFO(logger, "[" << r->getTypeStrShort() << " " << r->getId() << " " << r->getStatus() << (r->isSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG4CXX_ERROR_R(logger, r, ...) { LOG4CXX_ERROR(logger, "[" << r->getTypeStrShort() << " " << r->getId() << " " << r->getStatus() << (r->isSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG4CXX_FATAL_R(logger, r, ...) { LOG4CXX_FATAL(logger, "[" << r->getTypeStrShort() << " " << r->getId() << " " << r->getStatus() << (r->isSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }

#define LOG_TRACE_R(o, r, ...) { LOG4CXX_TRACE(o->getLogger(), o->getId() << ": [" << r->getTypeStrShort() << " " << r->getId() << " " << r->getStatus() << (r->isSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG_DEBUG_R(o, r, ...) { LOG4CXX_DEBUG(o->getLogger(), o->getId() << ": [" << r->getTypeStrShort() << " " << r->getId() << " " << r->getStatus() << (r->isSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG_INFO_R(o, r, ...) { LOG4CXX_INFO(o->getLogger(), o->getId() << ": [" << r->getTypeStrShort() << " " << r->getId() << " " << r->getStatus() << (r->isSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG_ERROR_R(o, r, ...) { LOG4CXX_ERROR(o->getLogger(), o->getId() << ": [" << r->getTypeStrShort() << " " << r->getId() << " " << r->getStatus() << (r->isSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }
#define LOG_FATAL_R(o, r, ...) { LOG4CXX_FATAL(o->getLogger(), o->getId() << ": [" << r->getTypeStrShort() << " " << r->getId() << " " << r->getStatus() << (r->isSetFlag(Resource::DELETED) ? "d" : "") << "] " << __VA_ARGS__) }

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
	virtual bool isProtobufResource();
	// save and restore resource
	virtual std::string *Serialize() = 0;
	// data need not be nul-terminated
	virtual bool Deserialize(const char *data, int size) = 0;
	// get info about a resource field
	virtual ResourceAttrInfo *GetAttrInfo(const char *name) = 0;
	// type id of a resource (to be used by Resources::AcquireResource(typeid))
	virtual int getTypeId() = 0;
	// type string of a resource
	virtual const char *getTypeStr() = 0;
	virtual const char *getTypeStrShort() = 0;
	// module prefix (e.g. Hector for Hector::TestResource)
	virtual const char *getModuleStr() = 0;
	// resource may contain link to other resource, it is only kept only in the memory
	Resource *getAttachedResource();
	void setAttachedResource(Resource *attachedResource);
	void clearAttachedResource();
	// used by queues in case there is limit on queue size, this size may
	// be somewhat arbitrary
	virtual int getSize() = 0;
	// return string representation of the resource (e.g. for debugging purposes)
	virtual std::string toString(Object::LogLevel = Object::INFO) = 0;
	std::string toStringShort();
	// id should be unique across all in-memory resources
	virtual int getId();
	virtual void setId(int id);
	// status may be tested in Processor to select target queue
	virtual int getStatus();
	virtual void setStatus(int status);
	// flags 
	void setFlag(Flag flag);
	void resetFlag(Flag flag);
	bool isSetFlag(Flag flag);

	// methods common to all Resources
	static int NextResourceId();

	// serializes resource (together with size and type), returns total bytes written
	static bool Serialize(Resource *resource, google::protobuf::io::CodedOutputStream *stream);
	static Resource *Deserialize(google::protobuf::io::CodedInputStream *stream, int *totalSize);

	static ResourceRegistry registry;

	// generic API for getting/setting of attributes
	virtual const std::string &GetAttrString(const char *name);
	virtual const std::string &GetAttrString(ResourceAttrInfo *ai);
	virtual int GetAttrInt(const char *name);
	virtual int GetAttrInt(ResourceAttrInfo *ai);
	virtual long GetAttrLong(const char *name);
	virtual long GetAttrLong(ResourceAttrInfo *ai);
	virtual const IpAddr &GetAttrIpAddr(const char *name);
	virtual const IpAddr &GetAttrIpAddr(ResourceAttrInfo *ai);
	virtual const std::string &GetAttrArrayString(const char *name, int index);
	virtual const std::string &GetAttrArrayString(ResourceAttrInfo *ai, int index);
	virtual int GetAttrArrayInt(const char *name, int index);
	virtual int GetAttrArrayInt(ResourceAttrInfo *ai, int index);
	virtual long GetAttrArrayLong(const char *name, int index);
	virtual long GetAttrArrayLong(ResourceAttrInfo *ai, int index);
	virtual const IpAddr &GetAttrArrayIpAddr(const char *name, int index);
	virtual const IpAddr &GetAttrArrayIpAddr(ResourceAttrInfo *ai, int index);
	virtual const std::string &GetAttrHashString(const char *name, const std::string &index);
	virtual const std::string &GetAttrHashString(ResourceAttrInfo *ai, const std::string &index);
	virtual int GetAttrHashInt(const char *name, const std::string &index);
	virtual int GetAttrHashInt(ResourceAttrInfo *ai, const std::string &index);
	virtual long GetAttrHashLong(const char *name, const std::string &index);
	virtual long GetAttrHashLong(ResourceAttrInfo *ai, const std::string &index);
	virtual const IpAddr &GetAttrHashIpAddr(const char *name, const std::string &index);
	virtual const IpAddr &GetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index);

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

protected:
	// memory-only, used just in Processor
	int flags;
	// these are memory-only, some resources may decide to keep them
	// on-disk too
	// N.B.: id should not be overwritten in Deserialize()
	int id;
	int status;
	Resource *attachedResource;

	static PlainLock idLock;
	static int nextId;

	// helper, so that we can return empty string reference
	static std::string empty;

	static log4cxx::LoggerPtr logger;
};

inline bool Resource::isProtobufResource() {
	return false;
}

inline int Resource::getId() {
	return id;
}

inline void Resource::setId(int id) {
	this->id = id;
}

inline int Resource::getStatus() {
	return status;
}

inline void Resource::setStatus(int status) {
	this->status = status;
}

inline void Resource::setFlag(Flag flag) {
	this->flags |= flag;
}

inline void Resource::resetFlag(Flag flag) {
	this->flags &= 0xFFFF ^ flag;
}

inline bool Resource::isSetFlag(Flag flag) {
	return (this->flags & flag) != 0;
}

inline void Resource::setAttachedResource(Resource *attachedResource) {
	this->attachedResource = attachedResource;
}

inline Resource *Resource::getAttachedResource() {
	return attachedResource;
}

inline void Resource::clearAttachedResource() {
	attachedResource = NULL;
}

inline std::string Resource::toStringShort() {
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "[%s %d %d]", getTypeStrShort(), getId(), isSetFlag(DELETED) ? -1 : getStatus());
	return buffer;
}

inline const std::string &Resource::GetAttrString(const char *name) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	return ai ? ai->getString(this) : empty;
}

inline const std::string &Resource::GetAttrString(ResourceAttrInfo *ai) {
	return ai->getString(this);
}

inline int Resource::GetAttrInt(const char *name) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	return ai->getInt(this);
}

inline int Resource::GetAttrInt(ResourceAttrInfo *ai) {
	return ai->getInt(this);
}

inline long Resource::GetAttrLong(const char *name) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	return ai->getLong(this);
}

inline long Resource::GetAttrLong(ResourceAttrInfo *ai) {
	return ai->getLong(this);
}

inline const IpAddr &Resource::GetAttrIpAddr(const char *name) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	return ai->getIpAddr(this);
}

inline const IpAddr &Resource::GetAttrIpAddr(ResourceAttrInfo *ai) {
	return ai->getIpAddr(this);
}

inline const std::string &Resource::GetAttrArrayString(const char *name, int index) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	return ai->getArrayString(this, index);
}

inline const std::string &Resource::GetAttrArrayString(ResourceAttrInfo *ai, int index) {
	return ai->getArrayString(this, index);
}

inline int Resource::GetAttrArrayInt(const char *name, int index) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	return ai->getArrayInt(this, index);
}

inline int Resource::GetAttrArrayInt(ResourceAttrInfo *ai, int index) {
	return ai->getArrayInt(this, index);
}

inline long Resource::GetAttrArrayLong(const char *name, int index) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	return ai->getArrayLong(this, index);
}

inline long Resource::GetAttrArrayLong(ResourceAttrInfo *ai, int index) {
	return ai->getArrayLong(this, index);
}

inline const IpAddr &Resource::GetAttrArrayIpAddr(const char *name, int index) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	return ai->getArrayIpAddr(this, index);
}

inline const IpAddr &Resource::GetAttrArrayIpAddr(ResourceAttrInfo *ai, int index) {
	return ai->getArrayIpAddr(this, index);
}

inline const std::string &Resource::GetAttrHashString(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	return ai->getHashString(this, index);
}

inline const std::string &Resource::GetAttrHashString(ResourceAttrInfo *ai, const std::string &index) {
	return ai->getHashString(this, index);
}

inline int Resource::GetAttrHashInt(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	return ai->getHashInt(this, index);
}

inline int Resource::GetAttrHashInt(ResourceAttrInfo *ai, const std::string &index) {
	return ai->getHashInt(this, index);
}

inline long Resource::GetAttrHashLong(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	return ai->getHashLong(this, index);
}

inline long Resource::GetAttrHashLong(ResourceAttrInfo *ai, const std::string &index) {
	return ai->getHashLong(this, index);
}

inline const IpAddr &Resource::GetAttrHashIpAddr(const char *name, const std::string &index) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	return ai->getHashIpAddr(this, index);
}

inline const IpAddr &Resource::GetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index) {
	return ai->getHashIpAddr(this, index);
}

inline void Resource::SetAttrString(const char *name, const std::string &value) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	if (ai)
		ai->setString(this, value);
}

inline void Resource::SetAttrString(ResourceAttrInfo *ai, const std::string &value) {
	ai->setString(this, value);
}

inline void Resource::SetAttrInt(const char *name, int value) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	if (ai)
		ai->setInt(this, value);
}

inline void Resource::SetAttrInt(ResourceAttrInfo *ai, int value) {
	ai->setInt(this, value);
}

inline void Resource::SetAttrLong(const char *name, long value) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	if (ai)
		ai->setLong(this, value);
}

inline void Resource::SetAttrLong(ResourceAttrInfo *ai, long value) {
	ai->setLong(this, value);
}

inline void Resource::SetAttrIpAddr(const char *name, IpAddr &value) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	if (ai)
		ai->setIpAddr(this, value);
}

inline void Resource::SetAttrIpAddr(ResourceAttrInfo *ai, IpAddr &value) {
	ai->setIpAddr(this, value);
}

inline void Resource::SetAttrArrayString(const char *name, int index, const std::string &value) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	if (ai)
		ai->setArrayString(this, index, value);
}

inline void Resource::SetAttrArrayString(ResourceAttrInfo *ai, int index, const std::string &value) {
	ai->setArrayString(this, index, value);
}

inline void Resource::SetAttrArrayInt(const char *name, int index, int value) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	if (ai)
		ai->setArrayInt(this, index, value);
}

inline void Resource::SetAttrArrayInt(ResourceAttrInfo *ai, int index, int value) {
	ai->setArrayInt(this, index, value);
}

inline void Resource::SetAttrArrayLong(const char *name, int index, long value) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	if (ai)
		ai->setArrayLong(this, index, value);
}

inline void Resource::SetAttrArrayLong(ResourceAttrInfo *ai, int index, long value) {
	ai->setArrayLong(this, index, value);
}

inline void Resource::SetAttrArrayIpAddr(const char *name, int index, IpAddr &value) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	if (ai)
		ai->setArrayIpAddr(this, index, value);
}

inline void Resource::SetAttrArrayIpAddr(ResourceAttrInfo *ai, int index, IpAddr &value) {
	ai->setArrayIpAddr(this, index, value);
}

inline void Resource::SetAttrHashString(const char *name, const std::string &index, const std::string &value) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	if (ai)
		ai->setHashString(this, index, value);
}

inline void Resource::SetAttrHashString(ResourceAttrInfo *ai, const std::string &index, const std::string &value) {
	ai->setHashString(this, index, value);
}

inline void Resource::SetAttrHashInt(const char *name, const std::string &index, int value) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	if (ai)
		ai->setHashInt(this, index, value);
}

inline void Resource::SetAttrHashInt(ResourceAttrInfo *ai, const std::string &index, int value) {
	ai->setHashInt(this, index, value);
}

inline void Resource::SetAttrHashLong(const char *name, const std::string &index, long value) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	if (ai)
		ai->setHashLong(this, index, value);
}

inline void Resource::SetAttrHashLong(ResourceAttrInfo *ai, const std::string &index, long value) {
	ai->setHashLong(this, index, value);
}

inline void Resource::SetAttrHashIpAddr(const char *name, const std::string &index, IpAddr &value) {
	ResourceAttrInfo *ai = GetAttrInfo(name);
	if (ai)
		ai->setHashIpAddr(this, index, value);
}

inline void Resource::SetAttrHashIpAddr(ResourceAttrInfo *ai, const std::string &index, IpAddr &value) {
	ai->setHashIpAddr(this, index, value);
}

#endif
