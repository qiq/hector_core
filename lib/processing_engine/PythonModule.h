/**
 * Dummy module, does nothing.
 */

#ifndef _LIB_PROCESSING_ENGINE_PYTHON_MODULE_H_
#define _LIB_PROCESSING_ENGINE_PYTHON_MODULE_H_

#include <config.h>

#ifdef HAVE_PYTHON_H

#include "EmbeddedPython.h"
#include "Module.h"

class PythonModule : public Module {
public:
	PythonModule(ObjectRegistry *objects, const char *id, int threadIndex, const char *name);
	~PythonModule();
	bool Init(std::vector<std::pair<std::string, std::string> > *args);
	Module::Type GetType();
	void StartSync();
	void StopSync();
	void PauseSync();
	void ResumeSync();
	Resource *ProcessInputSync(bool sleep);
	Resource *ProcessOutputSync(Resource *resource);
	Resource *ProcessSimpleSync(Resource *resource);
	int ProcessMultiSync(std::queue<Resource*> *inputResources, std::queue<Resource*> *outputResources, int *expectingResources);

protected:
	char *GetPropertySync(const char *name);
	bool SetPropertySync(const char *name, const char *value);
	std::vector<std::string> *ListPropertiesSync();

	void SaveCheckpointSync(const char *path, const char *id);
	void RestoreCheckpointSync(const char *path, const char *id);

	char *name;
	EmbeddedPython *python;
	PyObject *module;
	PyObject *obj;		// this module as an Object
};

#else

#include "Module.h"

using namespace std;

class PythonModule : public Module {
public:
	PythonModule(ObjectRegistry *objects, const char *id, int threadIndex, const char *name): Module(objects, id, threadIndex) {};
	~PythonModule() {};
};

#endif

#endif
