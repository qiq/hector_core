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

class ProcessingEngine;

class Module : public Object {
public:
	enum Type {
		INVALID = 0,
		INPUT =  1,
		OUTPUT = 2,
		SIMPLE = 3,
		MULTI =  4,
	};

	Module(ObjectRegistry *objects, ProcessingEngine *engine, const char *id, int threadIndex): Object(objects, id, threadIndex), engine(engine), threadIndex(threadIndex) {};
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
	virtual int ProcessMulti(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources);
	// Returns number of currently processing resources (in ProcessMulti)
	// called exclusively outside of ProcessMulti(), so there is no need to lock
	virtual int ProcessingResources();

	virtual void Start();
	virtual void Stop();
	virtual void Pause();
	virtual void Resume();

        // Helper methods for SWIG
        static void ResourceQueuePush(std::queue<Resource*> *queue, Resource *resource);
        static Resource *ResourceQueuePop(std::queue<Resource*> *queue);
        static int ResourceQueueSize(std::queue<Resource*> *queue);

protected:
	ProcessingEngine *engine;
	int threadIndex;
};

inline int Module::getThreadIndex() {
	return threadIndex;
}

// Helper methods for SWIG, FIXME: do we need this? We convert vector<Resource*> to Perl array anyway...

inline void Module::ResourceQueuePush(std::queue<Resource*> *queue, Resource *resource) {
	queue->push(resource);
}

inline Resource *Module::ResourceQueuePop(std::queue<Resource*> *queue) {
	Resource *resource = queue->front();
	queue->pop();
	return resource;
}

inline int Module::ResourceQueueSize(std::queue<Resource*> *queue) {
	return queue->size();
}
#endif
