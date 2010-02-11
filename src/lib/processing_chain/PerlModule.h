/**
 * Dummy module, does nothing.
 */

#ifndef _PERL_MODULE_H_
#define _PERL_MODULE_H_

#include <config.h>

#ifdef HAVE_PERL_H

#include <EXTERN.h>
#include <perl.h>
#undef New	// protocol buffers
#undef Pause	// Processor

#include "Module.h"

using namespace std;

class PerlModule : public Module {
public:
	PerlModule(ObjectRegistry *objects, const char *id, int threadIndex, const char *name);
	~PerlModule();
	bool Init(vector<pair<string, string> > *args);
	module_t getType();
	Resource *Process(Resource *resource);

protected:
	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	vector<string> *listNamesSync();

	void SaveCheckpointSync(const char *path, const char *id);
	void RestoreCheckpointSync(const char *path, const char *id);

	char *name;
	PerlInterpreter *my_perl;
	SV *ref;

	static log4cxx::LoggerPtr logger;
};

#else

#include "Module.h"

using namespace std;

class PerlModule : public Module {
public:
	PerlModule(ObjectRegistry *objects, const char *id, int threadIndex, const char *name): Module(objects, id, threadIndex) {};
	~PerlModule() {};
};

#endif

#endif
