/**
 * External simple
 */

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include "common.h"
#include "ModuleRPCSimple.h"

log4cxx::LoggerPtr ModuleRPCSimple::logger(log4cxx::Logger::getLogger("lib.processing_chain.ModuleRPCSimple"));

void buffer_destroy(void *buf) {
	delete (ExternalProcess*)buf;
}

ModuleExternalSimple::ModuleExternalSimple() {
	args = NULL;

	pthread_key_create(&buffer_key, buffer_destroy);
}

ModuleExternalSimple::~ModuleExternalSimple() {
	delete args;
}

bool ModuleExternalSimple::Init(Config *config, const char *name) {
	// path
	const char *path = config->getValue(name, "path");
	if (path == NULL) {
		LOG_CONFIG_ERROR0(logger, config->getXMLline(name, "threads"), config->getXMLcolumn(name, "threads"), "No path specified");
		return false;
	}
	this->path = path;

	// args
	int argN = config->getSize(name, "arg");
	args = new const char*[argN+2];
	args[0] = path;
	for (int i = 0; i < argN; i++) {
		const char *arg = config->getValue(name, "arg", i);
                assert(arg != NULL);
		args[i+1] = arg;
	}
	args[argN] = NULL;

	return true;
}

Resource *ModuleExternalSimple::Process(Resource *resource) {
	// get ExternalProcess
	ExternalProcess *process = (ExternalProcess*)pthread_getspecific(buffer_key);
	if (!process) {
		process = new ExternalProcess();
		if (!process->Init(path, args)) {
			LOG4CXX_ERROR(logger, "Cannot initialize external process");
			return NULL;
		}
		pthread_setspecific(buffer_key, process);
	}

	// write serialized Resource into output stream
	string *serial = resource->serialize();
	if (serial == NULL) {
		LOG4CXX_ERROR(logger, "Cannot serialize resource");
		return NULL;
	}
	const char *sout = serial->c_str();
	uint32_t len = htonl(serial->length());
	int result = process->readWrite((char *)&len, 4, NULL, 0);
	if (result != 4) {
		LOG4CXX_ERROR(logger, "Cannot write 4 bytes");
		// TODO: mark as broken
		delete serial;
		return NULL;
	}
	result = process->readWrite((char *)sout, len, size, 4);
	if (result != (int)len + 4) {
		LOG4CXX_ERROR(logger, "Cannot write resource/read 4 bytes");
		// TODO: mark as broken
		delete serial;
		return NULL;
	}
	delete serial;
	len = ntohl(size);
	if ((int)len > 1024*1024) {
		LOG4CXX_INFO(logger, "Too big resource");
		return NULL;
	}
	char *sin = new char[len];
	result = process->readWrite(NULL, 0, sin, len);
	if (result != (int)len) {
		LOG4CXX_ERROR(logger, "Cannot read resource");
		// TODO: mark as broken
		return NULL;
	}
	serial = new string(sin, len);
	delete sin;
	resource->deserialize(serial);
	delete serial;

	return resource;
}

void ModuleExternalSimple::createCheckpoint() {
}

// the class factories

extern "C" ModuleSimple* create() {
	return new ModuleExternalSimple();
}

extern "C" void destroy(ModuleSimple* p) {
	delete p;
}

