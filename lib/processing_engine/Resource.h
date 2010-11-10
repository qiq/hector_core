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

class ResourceFieldInfo;

class Resource {
public:
	Resource() {};
	virtual ~Resource() {};
	// create copy of a resource
	virtual Resource *Clone() = 0;
	// get info about a resource field
	virtual ResourceFieldInfo *getFieldInfo(const char *name) = 0;
	// type id of a resource (to be used by Resources::CreateResource(typeid))
	virtual int getTypeId() = 0;
	// type string of a resource
	virtual const char *getTypeStr() = 0;
	// module prefix (e.g. Hector for Hector::TestResource)
	virtual const char *getModuleStr() = 0;
	// id should be unique across all resources
	virtual int getId() = 0;
	virtual void setId(int id) = 0;
	// status may be tested in Processor to select target queue
	virtual int getStatus() = 0;
	virtual void setStatus(int status) = 0;
	void setStatusDeleted();
	bool isStatusDeleted();

	// save and restore resource
	virtual std::string *Serialize() = 0;
	// data need not be nul-terminated
	virtual bool Deserialize(const char *data, int size) = 0;
	// used by queues in case there is limit on queue size, this size may
	// be somewhat arbitrary
	virtual int getSize() = 0;
	// return string representation of the resource (e.g. for debugging purposes)
	virtual std::string *toString(Object::LogLevel = Object::INFO) = 0;

	static Resource *CreateResource(int id);
	static int NameToId(const char *name);
	static const int RESOURCE_DELETED;

protected:
	static int LoadResourceLibrary(const char *name, int id);

	static PlainLock lock;
	static std::tr1::unordered_map<std::string, int> name2id;
	static std::tr1::unordered_map<int, Resource *(*)()> id2create;
	static log4cxx::LoggerPtr logger;
};

inline void Resource::setStatusDeleted() {
	setStatus(RESOURCE_DELETED);
}

inline bool Resource::isStatusDeleted() {
	return getStatus() == RESOURCE_DELETED;
}

#endif
