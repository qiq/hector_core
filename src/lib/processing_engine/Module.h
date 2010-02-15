/**
 * Module, pure virtual class, ancestor of all modules.
 */

#ifndef _MODULE_H_
#define _MODULE_H_

#include <config.h>

#include <string>
#include <vector>
#include <log4cxx/logger.h>
#include "Object.h"
#include "Resource.h"

typedef enum {
	MODULE_INVALID = 0,
	MODULE_INPUT =  1,
	MODULE_OUTPUT = 2,
	MODULE_SIMPLE = 3,
	MODULE_MULTI =  4,
	MODULE_SELECT = 5 
} module_t;

#define MODULE_LOG_TRACE(logger, ...) LOG4CXX_TRACE(logger, getId() << "[" << getThreadIndex() << "]: " << __VA_ARGS__)
#define MODULE_LOG_DEBUG(logger, ...) LOG4CXX_DEBUG(logger, getId() << "[" << getThreadIndex() << "]: " << __VA_ARGS__)
#define MODULE_LOG_INFO(logger, ...) LOG4CXX_INFO(logger, getId() << "[" << getThreadIndex() << "]: " << __VA_ARGS__)
#define MODULE_LOG_ERROR(logger, ...) LOG4CXX_ERROR(logger, getId() << "[" << getThreadIndex() << "]: " << __VA_ARGS__)
#define MODULE_LOG_FATAL(logger, ...) LOG4CXX_FATAL(logger, getId() << "[" << getThreadIndex() << "]: " << __VA_ARGS__)

class Module : public Object {
public:
	Module(ObjectRegistry *objects, const char *id, int threadIndex): Object(objects, id), threadIndex(threadIndex) {};
	virtual ~Module() {};
	virtual bool Init(vector<pair<string, string> > *args);
	virtual module_t getType();
	int getThreadIndex();
	//virtual int busyResources() = 0;
	virtual Resource *Process(Resource *resource);
	virtual int Process(Resource **in, Resource **out);

protected:
	int threadIndex;

	static log4cxx::LoggerPtr logger;
};

inline int Module::getThreadIndex() {
	return threadIndex;
}

#endif
