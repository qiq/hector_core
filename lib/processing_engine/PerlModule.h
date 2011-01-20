/**
 * Dummy module, does nothing.
 */

#ifndef _LIB_PROCESSING_ENGINE_PERL_MODULE_H_
#define _LIB_PROCESSING_ENGINE_PERL_MODULE_H_

#include <config.h>

#ifdef HAVE_PERL_H

#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>
#undef New	// protocol buffers
#undef Pause	// Processor

#include <tr1/unordered_set>

#include "Module.h"

class PerlModule : public Module {
public:
	PerlModule(ObjectRegistry *objects, const char *id, int threadIndex, const char *name);
	~PerlModule();
	bool Init(std::vector<std::pair<std::string, std::string> > *args);
	Module::Type getType();
	Resource *ProcessInputSync(bool sleep);
	Resource *ProcessOutputSync(Resource *resource);
	Resource *ProcessSimpleSync(Resource *resource);
	int ProcessMultiSync(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, int *expectingResources);

protected:
	char *GetValueSync(const char *name);
	bool SetValueSync(const char *name, const char *value);
	std::vector<std::string> *ListNamesSync();

	void SaveCheckpointSync(const char *path, const char *id);
	void RestoreCheckpointSync(const char *path, const char *id);
	
	SV *CreatePerlResource(Resource *resource);

	char *name;
	PerlInterpreter *my_perl;
	SV *ref;

	std::tr1::unordered_set<std::string> initialized;
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
