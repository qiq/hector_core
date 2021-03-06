/**
 * Dummy module, does nothing.
 */

#ifndef _LIB_PROCESSING_ENGINE_PERL_MODULE_H_
#define _LIB_PROCESSING_ENGINE_PERL_MODULE_H_

#include <config.h>

#ifdef HAVE_PERL_H

#include "EmbeddedPerl.h"

#include "Module.h"

class PerlModule : public Module {
public:
	PerlModule(ObjectRegistry *objects, const char *id, int threadIndex, const char *name);
	~PerlModule();
	bool Init(std::vector<std::pair<std::string, std::string> > *args);
	Module::Type GetType();
	void StartSync();
	void StopSync();
	void PauseSync();
	void ResumeSync();
	Resource *ProcessInputSync(bool sleep);
	Resource *ProcessOutputSync(Resource *resource);
	Resource *ProcessSimpleSync(Resource *resource);
	bool ProcessMultiSync(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, int *expectingResources, int *processingResources);

protected:
	char *GetPropertySync(const char *name);
	bool SetPropertySync(const char *name, const char *value);
	std::vector<std::string> *ListPropertiesSync();

	void SaveCheckpointSync(const char *path, const char *id);
	void RestoreCheckpointSync(const char *path, const char *id);
	
	SV *CreatePerlResource(Resource *resource);

	char *name;
	EmbeddedPerl *perl;
	SV *ref;
};

#else

#include "Module.h"

class PerlModule : public Module {
public:
	PerlModule(ObjectRegistry *objects, const char *id, int threadIndex, const char *name): Module(objects, id, threadIndex) {};
	~PerlModule() {};
};

#endif

#endif
