/**
 * Class representing a resource, basic item of processing.
 */

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <config.h>

#include <string>
#include <log4cxx/logger.h>

using namespace std;

class Resource {
public:
	Resource() {};
	virtual ~Resource() {};
	// create copy of a resource
	virtual Resource *Clone() = 0;
	// type id of a resource (to be used by Resources::CreateResource(typeid))
	virtual int getType() = 0;
	// id should be unique across all resources
	virtual int getId() = 0;
	virtual void setId(int id) = 0;
	// status may be tested in Processor to select target queue
	virtual int getStatus() = 0;
	virtual void setStatus(int status) = 0;
	// save and restore resource
	virtual string *serialize() = 0;
	virtual bool deserialize(string *s) = 0;
	// used by queues in case there is limit on queue size
	virtual int getSize() = 0;
protected:
	static log4cxx::LoggerPtr logger;
};

#endif
