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
	void Start();
	void Stop();
	void Pause();
	void Resume();
	Resource *ProcessInput(bool sleep);
	Resource *ProcessOutput(Resource *resource);
	Resource *ProcessSimple(Resource *resource);
	int ProcessMulti(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, int *expectingResources);

protected:
	int threadIndex;

	virtual void StartSync();
	virtual void StopSync();
	virtual void PauseSync();
	virtual void ResumeSync();

	virtual Resource *ProcessInputSync(bool sleep);
	// Output module: consume resource, return resource to be deleted
	virtual Resource *ProcessOutputSync(Resource *resource);
	// Simple module: one resource a time
	virtual Resource *ProcessSimpleSync(Resource *resource);
	// Multi modules use input/output interface: inputResources should be
	// consumed, outputResources should be produced Returns number of
	// currently processing resources (not counting inputResources and
	// outputResources). expectingResources is number of resources module is
	// expecting on the input next time. ProcessMulti is allowed to produce
	// new resources
	virtual int ProcessMultiSync(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, int *expectingResources);
};

inline int Module::getThreadIndex() {
	return threadIndex;
}

inline void Module::Start() {
	ObjectLockWrite();
	StartSync();
	ObjectUnlock();
}

inline void Module::Stop() {
	ObjectLockWrite();
	StopSync();
	ObjectUnlock();
}

inline void Module::Pause() {
	ObjectLockWrite();
	PauseSync();
	ObjectUnlock();
}

inline void Module::Resume() {
	ObjectLockWrite();
	ResumeSync();
	ObjectUnlock();
}

inline Resource *Module::ProcessInput(bool sleep) {
	ObjectLockWrite();
	Resource *result = ProcessInputSync(sleep);
	ObjectUnlock();
	return result;
}

inline Resource *Module::ProcessOutput(Resource *resource) {
	ObjectLockWrite();
	resource = ProcessOutputSync(resource);
	ObjectUnlock();
	return resource;
}

inline Resource *Module::ProcessSimple(Resource *resource) {
	ObjectLockWrite();
	resource = ProcessSimpleSync(resource);
	ObjectUnlock();
	return resource;
}

inline int Module::ProcessMulti(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, int *expectingResources) {
	ObjectLockWrite();
	int result = ProcessMultiSync(inputResources, outputResources, expectingResources);
	ObjectUnlock();
	return result;
}

#endif
