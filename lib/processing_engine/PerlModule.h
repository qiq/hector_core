/**
 * Dummy module, does nothing.
 */

#ifndef _PERL_MODULE_H_
#define _PERL_MODULE_H_

#include <config.h>

#ifdef HAVE_PERL_H

#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>
#undef New	// protocol buffers
#undef Pause	// Processor

#include <tr1/unordered_set>

#include "Module.h"

using namespace std;

class PerlModule : public Module {
public:
	PerlModule(ObjectRegistry *objects, const char *id, int threadIndex, const char *name);
	~PerlModule();
	bool Init(vector<pair<string, string> > *args);
	Module::Type getType();
	Resource *Process(Resource *resource);
	int ProcessMulti(queue<Resource*> *inputResources, queue<Resource*> *outputResources);

protected:
	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	vector<string> *listNamesSync();

	void SaveCheckpointSync(const char *path, const char *id);
	void RestoreCheckpointSync(const char *path, const char *id);
	
	SV *CreatePerlResource(Resource *resource);

	char *name;
	PerlInterpreter *my_perl;
	SV *ref;

	std::tr1::unordered_set<string> initialized;

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
