/**
 * Module, pure virtual class, ancestor of all modules.
 */

#ifndef _LIB_PROCESSING_ENGINE_MODULE_H_
#define _LIB_PROCESSING_ENGINE_MODULE_H_

#include <config.h>

#include <string>
#include <vector>
#include <queue>
#include "Object.h"
#include "Resource.h"

class Module : public Object {
public:
	enum Type {
		INVALID = 0,
		INPUT =  1,
		OUTPUT = 2,
		SIMPLE = 3,
		MULTI =  4,
	};

	Module(ObjectRegistry *objects, const char *id, int threadIndex): Object(objects, id, threadIndex), threadIndex(threadIndex) {};
	virtual ~Module() {};
	virtual bool Init(std::vector<std::pair<std::string, std::string> > *args);
	virtual Module::Type getType();
	int getThreadIndex();
	// Input module: produce resource, possibly wait for input
	virtual Resource *ProcessInput(bool sleep);
	// Output module: consume resource
	virtual void ProcessOutput(Resource *resource);
	// Simple module: one resource a time
	virtual Resource *ProcessSimple(Resource *resource);
	// Multi modules use input/output interface: inputResources should be consumed, outputResources should be produced
	// returns number of resources we are expecting on the input, -1 in case we should block in waiting for input resources
	virtual int ProcessMulti(queue<Resource*> *inputResources, queue<Resource*> *outputResources);
	virtual int ProcessingResources();
	virtual void Start();
	virtual void Stop();
	virtual void Pause();
	virtual void Resume();

protected:
	int threadIndex;
};

inline int Module::getThreadIndex() {
	return threadIndex;
}

#endif
