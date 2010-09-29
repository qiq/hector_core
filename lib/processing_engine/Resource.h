/**
 * Class representing a resource, basic item of processing.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>
#include "Object.h"

using namespace std;

class Resource {
public:
	Resource() {};
	virtual ~Resource() {};
	// create copy of a resource
	virtual Resource *Clone() = 0;
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

	// save and restore resource
	virtual string *Serialize() = 0;
	virtual bool Deserialize(string *s) = 0;
	// used by queues in case there is limit on queue size, this size may
	// be somewhat arbitrary
	virtual int getSize() = 0;
	// return string representation of the resource (e.g. for debugging purposes)
	virtual char *toString(Object::LogLevel = Object::INFO) = 0;

	typedef enum {
		UNKNOWN,
		STRING,
		INT,
		LONG,
		IP4,
		IP6,
		STRING2,
	} FieldType;

protected:
	static log4cxx::LoggerPtr logger;
};

#endif
