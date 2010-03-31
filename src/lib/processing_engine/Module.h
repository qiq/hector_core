/**
 * Module, pure virtual class, ancestor of all modules.
 */

#ifndef _MODULE_H_
#define _MODULE_H_

#include <config.h>

#include <string>
#include <vector>
#include <deque>
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

class Module : public Object {
public:
	Module(ObjectRegistry *objects, const char *id, int threadIndex): Object(objects, id, threadIndex), threadIndex(threadIndex) {};
	virtual ~Module() {};
	virtual bool Init(vector<pair<string, string> > *args);
	virtual module_t getType();
	int getThreadIndex();
	// Simple/Input/Output modules does only use this interface (one resource a time)
	virtual Resource *Process(Resource *resource);
	// Multi/Select modules use input/output interface
	virtual int Process(deque<Resource*> *inputResources, deque<Resource*> *outputResources);
	virtual void Start();
	virtual void Stop();
	virtual void Pause();
	virtual void Resume();

protected:
	int threadIndex;

	static log4cxx::LoggerPtr logger;
};

inline int Module::getThreadIndex() {
	return threadIndex;
}

#endif
