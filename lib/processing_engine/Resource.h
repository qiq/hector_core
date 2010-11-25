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

// logger helper macros (print short info about the resource)

#define LOG4CXX_TRACE_R(logger, r, ...) { LOG4CXX_TRACE(logger, "[" << r->getTypeStrShort() << " " << r->getId() << " " << (r->isStatusDeleted() ? -1 : r->getStatus()) << "] " << __VA_ARGS__) }
#define LOG4CXX_DEBUG_R(logger, r, ...) { LOG4CXX_DEBUG(logger, "[" << r->getTypeStrShort() << " " << r->getId() << " " << (r->isStatusDeleted() ? -1 : r->getStatus()) << "] " << __VA_ARGS__) }
#define LOG4CXX_INFO_R(logger, r, ...) { LOG4CXX_INFO(logger, "[" << r->getTypeStrShort() << " " << r->getId() << " " << (r->isStatusDeleted() ? -1 : r->getStatus()) << "] " << __VA_ARGS__) }
#define LOG4CXX_ERROR_R(logger, r, ...) { LOG4CXX_ERROR(logger, "[" << r->getTypeStrShort() << " " << r->getId() << " " << (r->isStatusDeleted() ? -1 : r->getStatus()) << "] " << __VA_ARGS__) }
#define LOG4CXX_FATAL_R(logger, r, ...) { LOG4CXX_FATAL(logger, "[" << r->getTypeStrShort() << " " << r->getId() << " " << (r->isStatusDeleted() ? -1 : r->getStatus()) << "] " << __VA_ARGS__) }

#define LOG_TRACE_R(r, ...) { LOG4CXX_TRACE(logger, getId() << ": [" << r->getTypeStrShort() << " " << r->getId() << " " << (r->isStatusDeleted() ? -1 : r->getStatus()) << "] " << __VA_ARGS__) }
#define LOG_DEBUG_R(r, ...) { LOG4CXX_DEBUG(logger, getId() << ": [" << r->getTypeStrShort() << " " << r->getId() << " " << (r->isStatusDeleted() ? -1 : r->getStatus()) << "] " << __VA_ARGS__) }
#define LOG_INFO_R(r, ...) { LOG4CXX_INFO(logger, getId() << ": [" << r->getTypeStrShort() << " " << r->getId() << " " << (r->isStatusDeleted() ? -1 : r->getStatus()) << "] " << __VA_ARGS__) }
#define LOG_ERROR_R(r, ...) { LOG4CXX_ERROR(logger, getId() << ": [" << r->getTypeStrShort() << " " << r->getId() << " " << (r->isStatusDeleted() ? -1 : r->getStatus()) << "] " << __VA_ARGS__) }
#define LOG_FATAL_R(r, ...) { LOG4CXX_FATAL(logger, getId() << ": [" << r->getTypeStrShort() << " " << r->getId() << " " << (r->isStatusDeleted() ? -1 : r->getStatus()) << "] " << __VA_ARGS__) }

class ResourceFieldInfo;

class Resource {
public:
	Resource();
	Resource(const Resource &r);
	virtual ~Resource() {};
	// create copy of a resource
	virtual Resource *Clone() = 0;
	// save and restore resource
	virtual std::string *Serialize() = 0;
	// data need not be nul-terminated
	virtual bool Deserialize(const char *data, int size) = 0;
	// get info about a resource field
	virtual ResourceFieldInfo *getFieldInfo(const char *name) = 0;
	// type id of a resource (to be used by Resources::CreateResource(typeid))
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
	void setStatusDeleted();
	bool isStatusDeleted();
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

	// static methods common to all Resources
	static Resource *CreateResource(int id);
	static int NameToId(const char *name);
	static const int RESOURCE_DELETED;

protected:
	// these are memory-only, some resources may decide to keep them
	// on-disk too
	// N.B.: id should not be overwritten in Deserialize()
	int id;
	int status;
	Resource *attachedResource;

	static int LoadResourceLibrary(const char *name, int id);
	static PlainLock translateLock;
	static std::tr1::unordered_map<std::string, int> name2id;
	static std::tr1::unordered_map<int, Resource *(*)()> id2create;

	static PlainLock idLock;
	static int nextId;

	static log4cxx::LoggerPtr logger;
};

inline void Resource::setStatusDeleted() {
	setStatus(RESOURCE_DELETED);
}

inline bool Resource::isStatusDeleted() {
	return getStatus() == RESOURCE_DELETED;
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
	snprintf(buffer, sizeof(buffer), "[%s %d %d]", getTypeStrShort(), getId(), isStatusDeleted() ? -1 : getStatus());
	return buffer;
}

#endif
