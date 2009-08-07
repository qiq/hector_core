/**
 * Dummy module, does nothing.
 */

#ifndef _MODULE_DUMMY_H_
#define _MODULE_DUMMY_H_

#include <config.h>

#include <log4cxx/logger.h>
#include "Module.h"

class Server;

class ModuleDummy : public Module {
public:
	ModuleDummy() {}
	~ModuleDummy() {}
	bool Init(Server *server, Config *config, const char *id);
	module_t getType();
	void Process(Resource *resource);
	void createCheckpoint();

	const char *getValue(const char *name);
	bool setValue(const char *name, const char *value);
};

inline module_t ModuleDummy::getType() {
	return MODULE_SIMPLE;
}

#endif
