/**
 * External simple
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include "common.h"
#include "../../resources/Resources.h"
#include "RPCSimpleModule.h"

log4cxx::LoggerPtr RPCSimpleModule::logger(log4cxx::Logger::getLogger("lib.processing_chain.RPCSimpleModule"));

RPCSimpleModule::RPCSimpleModule(ObjectRegistry *objects, const char *id): Module(objects, id) {
	rpc = NULL;
}

RPCSimpleModule::~RPCSimpleModule() {
	delete rpc;
}

bool RPCSimpleModule::Init(vector<pair<string, string> > *args) {
	LOG4CXX_ERROR(logger, "Init(args) not supported in this module");
	return false;
}

bool RPCSimpleModule::Init(RPC *rpc, vector<pair<string, string> > *c) {
	this->rpc = rpc;

	bool result = false;
	RPCMessage m;
	m.set_Init(c);
	if (rpc->SendReceiveMessage(&m)) {
		if (m.get_response_Init())
			result = true;
		else
			LOG4CXX_ERROR(logger, "Init() failed");
	}
	return result;
}

Resource *RPCSimpleModule::Process(Resource *resource) {
	// write serialized Resource into output stream
	string *serialIn = resource->serialize();
	if (serialIn == NULL) {
		LOG4CXX_ERROR(logger, "Cannot serialize resource");
		delete resource;
		return NULL;
	}
	resource_t type = resource->getType();
	string *serialOut = rpc->SendReceive((int *)&type, serialIn);
	delete serialIn;
	if (type < 0 || !serialOut) {
		delete resource;
		return NULL;
	}
	Resource *resourceOut = NULL;
	if (resource->getType() == type) {
		// reuse resource, if possible
		if (!resource->deserialize(serialOut)) {
			LOG4CXX_ERROR(logger, "Cannot deserialize resource");
			delete serialOut;
			delete resource;
			return NULL;
		}
	} else {
		Resource *resourceOut = ConstructResource(type, serialOut);
		if (!resourceOut) {
			LOG4CXX_ERROR(logger, "Invalid resource type: " << type);
			delete serialOut;
			delete resourceOut;
			delete resource;
			return NULL;
		}

		resourceOut->setAnotherResource(resource->getAnotherResource());
		resource->setAnotherResource(NULL);
		delete resource;
	}

	return resourceOut;
}

void RPCSimpleModule::SaveCheckpoint(const char *path, const char *id) {
	RPCMessage m;
	m.set_SaveCheckpoint(path, id);
	rpc->SendReceiveMessage(&m);
}

void RPCSimpleModule::RestoreCheckpoint(const char *path, const char *id) {
	RPCMessage m;
	m.set_RestoreCheckpoint(path, id);
	rpc->SendReceiveMessage(&m);
}

char *RPCSimpleModule::getValue(const char *name) {
	RPCMessage m;
	m.set_getValue(name);
	if (!rpc->SendReceiveMessage(&m)) {
		LOG4CXX_ERROR(logger, "Error getting value " << name);
		return NULL;
	}
	return strdup(m.get_response_getValue());
}

bool RPCSimpleModule::setValue(const char *name, const char *value) {
	RPCMessage m;
	m.set_setValue(name, value);
	if (!rpc->SendReceiveMessage(&m)) {
		LOG4CXX_ERROR(logger, "Error setting value " << name);
		return false;
	}
	return m.get_response_setValue();
}

vector<string> *RPCSimpleModule::listNames() {
	RPCMessage m;
	m.set_listNames();
	if (!rpc->SendReceiveMessage(&m)) {
		LOG4CXX_ERROR(logger, "Error getting names");
		return NULL;
	}
	vector<string> *result = new vector<string>();
	int i = 0;
	const char *name = m.get_response_listNames(i);
	while (name) {
		result->push_back(name);
		name = m.get_response_listNames(++i);
	}
	return result;
}
