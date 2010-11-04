/**
 * CallProcessingEngine: call another processing engine module template. Module
 * needs to implement PrepareResource and FinishResource methods and call ReadWrite() as appropriate
 */

#ifndef _LIB_PROCESSING_ENGINE_CALL_PROCESSING_ENGINE_MODULE_H_
#define _LIB_PROCESSING_ENGINE_CALL_PROCESSING_ENGINE_MODULE_H_

#include <config.h>

#include <tr1/unordered_map>
#include <unbound.h>
#include "common.h"
#include "Module.h"
#include "ObjectValues.h"

typedef struct CallResourceInfo_ {
	Resource *src;	// currently processed resource
	Resource *qr;	// query/response resource
} CallResourceInfo;

class CallProcessingEngineModule : public Module {
public:
	CallProcessingEngineModule(ObjectRegistry *objects, ProcessingEngine *engine, const char *id, int threadIndex);
	~CallProcessingEngineModule();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type getType();
	int ProcessMulti(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources);
	int ProcessingResources();

protected:
	// returns NULL in case src resource cannot be processed (e.g. is incompatible)
	virtual Resource *PrepareResource(Resource *src) = 0;
	virtual void FinishResource(Resource *src, Resource *qr) = 0;

	ObjectValues<CallProcessingEngineModule> *values;

private:
	int items;		// ObjectLock, items processed
	int maxRequests;	// initOnly, number of concurrent requests
	int timeTick;		// ObjectLock, max time to spend in ProcessMulti()

	ProcessingEngine *targetEngine;
	CallResourceInfo *tmpInputResource;
	std::vector<CallResourceInfo*> unused;
	std::tr1::unordered_map<int, CallResourceInfo*> running;

	char *getItems(const char *name);
	char *getMaxRequests(const char *name);
	void setMaxRequests(const char *name, const char *value);
	char *getTimeTick(const char *name);
	void setTimeTick(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	bool isInitOnly(const char *name);
	std::vector<std::string> *listNamesSync();

	bool ReadWrite(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, struct timeval *tv);
};

inline Module::Type CallProcessingEngineModule::getType() {
	return MULTI;
}

inline char *CallProcessingEngineModule::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool CallProcessingEngineModule::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline bool CallProcessingEngineModule::isInitOnly(const char *name) {
	return values->isInitOnly(name);
}

inline std::vector<std::string> *CallProcessingEngineModule::listNamesSync() {
	return values->listNamesSync();
}

#endif
