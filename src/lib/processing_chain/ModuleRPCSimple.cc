/**
 * External simple
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include "common.h"
#include "ModuleRPCSimple.h"
#include "Resources.h"
#include "RPCMessage.h"

log4cxx::LoggerPtr ModuleRPCSimple::logger(log4cxx::Logger::getLogger("lib.processing_chain.ModuleRPCSimple"));

ModuleRPCSimple(ObjectRegistry *objects, const char *id): Module(objects, id) {
	messageId = 0;
}

ModuleRPCSimple::~ModuleRPCSimple() {
	delete process;
}

bool ModuleRPCSimple::Init(vector<pair<string, string> > *c) {
	string path;
	vector<string> args;
	vector<string> envs;

	for (vector<pair<string, string> >::iterator iter = args->begin(); iter != args->end(); ++iter) {
		if (iter->first == "path") {
			path = iter->second;
		} else if (iter->first == "arg") {
			args.push_back(iter->second);
		} else if (iter->first == "env") {
			envs.push_back(iter->second);
		}
	}

	// argv
	int n = args.size();
	const char **argv = new const char*[n+2];
	argv[0] = path.c_str();
	for (int i = 0; i < n; i++) {
		argv[i+1] = args[i].c_str();
	}
	argv[n] = NULL;

	// env
	n = envs.size();
	const char **envv = new const char*[n+1];
	for (int i = 0; i < n; i++) {
		envv[i] = envs[i].c_str();
	}
	envv[n] = NULL;

	process = new ExternalProcess();
	if (!process->Init(path.c_str(), argv, envv)) {
		LOG4CXX_ERROR(logger, "Cannot initialize external process");
		return false;
	}

	delete argv;
	delete envv;

	bool result = false;
	RPCMessage m;
	m.set_Init(c);
	if (SendMessage(m)) {
		if (m.get_response_Init())
			result = true;
		else
			LOG4CXX_ERROR(logger, "Init() failed");
	}
	return result;
}

bool ModuleRPCSimple::Send(int type, int id, string *serialIn) {
	message_header_t header;
	header.type = type;
	header.id = id;
	header.length = serialIn->length();
	int result = process->ReadWrite((char *)&header, sizeof(header), NULL, 0);
	if (result != sizeof(header)) {
		LOG4CXX_ERROR(logger, "Cannot write header");
		return false;
	}
	result = process->ReadWrite((char *)serialIn->c_str(), serialIn->length(), NULL, 0);
	if (result != serialIn->length()) {
		LOG4CXX_ERROR(logger, "Cannot write data");
		return false;
	}
	return true;
}

bool ModuleRPCSimple::Receive(int *type, int *id, string *serialOut) {
	message_header_t header;
	result = process->ReadWrite(NULL, 0, (char *)&header, sizeof(header));
	if (result != sizeof(header)) {
		LOG4CXX_ERROR(logger, "Cannot read header");
		return false;
	}
	char *sin = new char[header.length];
	result = process->ReadWrite(NULL, 0, sin, header.length);
	if (result != (int)header.length) {
		LOG4CXX_ERROR(logger, "Cannot read resource");
		return false;
	}

	serialOut->assign(sin, len);
	*type = header.type;
	*id = header.id;
	return true;
}

int ModuleRPCSimple::SendReceive(int type, int id, string *serialIn, string *serialOut) {
	if (!Send(type, id, serialIn))
		return -1;
	int typeOut, idOut;
	if (!Receive(&typeOut, &idOut, serialOut))
		return -1;
	if (idOut != id) {
		LOG4CXX_ERROR(logger, "Invalid message id");
		return -1;
	}
	return typeOut;
}

Resource *ModuleRPCSimple::Process(Resource *resource) {
	// write serialized Resource into output stream
	string *serialIn = resource->serialize();
	if (serialIn == NULL) {
		LOG4CXX_ERROR(logger, "Cannot serialize resource");
		delete resource;
		return NULL;
	}
	string *serialOut = new string();
	int type = SendReceive(resource->getType(), messageId++, serialIn, serialOut);
	delete serialIn;
	if (type < 0) {
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
		Resource *resourceOut = ConstructResource(type);
		if (!resourceOut) {
			LOG4CXX_ERROR(logger, "Invalid resource type: " << type);
			delete serialOut;
			delete resourceOut;
			delete resource;
			return NULL;
		}
		if (!resourceOut->deserialize(serialOut)) {
			delete serialOut;
			delete resourceOut;
			delete resource;
			return NULL;
		}

		resourceOut->setAnotherResource(resource->getAnotherResource);
		resource->setAnotherResource(NULL);
		delete resource;
	}

	return resourceOut;
}

bool ModuleRPCSimple::SendReceiveMessage(RPCMessage *message) {
	string *serialIn = message->serialize();
	string *serialOut = string();
	int type = SendReceive(0, messageId++, serialIn, serialOut);
	if (type < 0)
		return false;
	return true;
}

void ModuleRPCSimple::SaveCheckpoint(const char *path, const char *id) {
	RPCMessage m;
	m.set_SaveCheckpoint(path, id);
	SendReceiveMessage(m);
}

void ModuleRPCSimple::RestoreCheckpoint(const char *path, const char *id) {
	RPCMessage m;
	m.set_RestoreCheckpoint(path, id);
	SendReceiveMessage(m);
}

char *ModuleRPCSimple::getValue(const char *name) {
	RPCMessage m;
	m.set_getValue(name);
	if (!SendReceiveMessage(m)) {
		LOG4CXX_ERROR(logger, "Error getting value " << name);
		return NULL;
	}
	return strdup(m.get_response_getValue());
}

bool ModuleRPCSimple::setValue(const char *name, const char *value) {
	RPCMessage m;
	m.set_setValue(name, value);
	if (!SendReceiveMessage(m)) {
		LOG4CXX_ERROR(logger, "Error setting value " << name);
		return false;
	}
	return get_response_setValue();
}

vector<string> *ModuleRPCSimple::listNames() {
	RPCMessage m;
	m.set_setlistNames();
	if (!SendReceiveMessage(m)) {
		LOG4CXX_ERROR(logger, "Error getting names");
		return NULL;
	}
	vector<string> *result = new vector<string>();
	int i = 0;
	const char *name = get_response_listNames(i);
	while (name) {
		result->push_back(name);
		name = get_response_listNames(++i);
	}
	return result;
}
