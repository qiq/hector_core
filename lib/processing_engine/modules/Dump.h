/**
Dump.la, simple, native
Dumps Resource content into log. Useful for debugging purposes.

Dependencies: pcrecpp
Parameters:
*/

#ifndef _LIB_PROCESSING_ENGINE_MODULES_DUMP_H_
#define _LIB_PROCESSING_ENGINE_MODULES_DUMP_H_

#include <config.h>

#include <string>
#include "common.h"
#include "Module.h"
#include "ObjectValues.h"

class Dump : public Module {
public:
	Dump(ObjectRegistry *objects, const char *id, int threadIndex);
	~Dump();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type getType();
	Resource *ProcessSimple(Resource *resource);

private:
	ObjectValues<Dump> *values;
	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	bool isInitOnly(const char *name);
	std::vector<std::string> *listNamesSync();
};

inline 	Module::Type Dump::getType() {
	return SIMPLE;
}

inline char *Dump::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool Dump::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline bool Dump::isInitOnly(const char *name) {
	return values->isInitOnly(name);
}

inline std::vector<std::string> *Dump::listNamesSync() {
	return values->listNamesSync();
}

#endif
