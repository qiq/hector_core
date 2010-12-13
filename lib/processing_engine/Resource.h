/**
 * Class representing a resource, basic item of processing.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_H_

#include <config.h>

#include <limits>
#include <string>
#include <tr1/unordered_map>
#include <log4cxx/logger.h>
#include "Object.h"
#include "PlainLock.h"

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

class ResourceFieldInfo;
class ResourceInfo;

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
	// save and restore resource
	virtual std::string *Serialize() = 0;
	// data need not be nul-terminated
	virtual bool Deserialize(const char *data, int size) = 0;
	// get info about a resource field
	virtual ResourceFieldInfo *getFieldInfo(const char *name) = 0;
	// type id of a resource (to be used by Resources::AcquireResource(typeid))
	virtual int getTypeId() = 0;
	// type string of a resource
	virtual const char *getTypeStr() = 0;
	virtual const char *getTypeStrShort() = 0;
	// module prefix (e.g. Hector for Hector::TestResource)
	virtual const char *getModuleStr() = 0;
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

	// methods common to all Resources
	static int NameToId(const char *name);
	static int NextResourceId();
	static Resource *AcquireResource(int id);
	static void ReleaseResource(Resource *resource);

protected:
	// memory-only, used just in Processor
	int flags;
	// these are memory-only, some resources may decide to keep them
	// on-disk too
	// N.B.: id should not be overwritten in Deserialize()
	int id;
	int status;
	Resource *attachedResource;

	static int LoadResourceLibrary(const char *name, int id);
	static PlainLock translateLock;
	static std::tr1::unordered_map<std::string, int> name2id;
	static std::tr1::unordered_map<int, ResourceInfo*> id2info;

	static PlainLock idLock;
	static int nextId;

	static log4cxx::LoggerPtr logger;
};

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

#endif
