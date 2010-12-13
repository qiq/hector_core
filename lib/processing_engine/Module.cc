//
#include <config.h>

#include "common.h"
#include "Module.h"

using namespace std;

bool Module::Init(vector<pair<string, string> > *args) {
	LOG_ERROR(this, "Unimplemented module " << getId() << " (Init)");
	return false;
}

Module::Type Module::getType() {
	LOG_ERROR(this, "Unimplemented module " << getId() << " (getType)");
	return Module::INVALID;
}

Resource *Module::ProcessInput(bool sleep) {
	LOG_ERROR(this, "Unimplemented resource processing (ProcessInput)");
	return NULL;
}

Resource *Module::ProcessOutput(Resource *resource) {
	LOG_ERROR(this, "Unimplemented resource processing (ProcessOutput)");
	return NULL;
}

Resource *Module::ProcessSimple(Resource *resource) {
	LOG_ERROR(this, "Unimplemented resource processing (ProcessSimple)");
	return NULL;
}

int Module::ProcessMulti(queue<Resource*> *inputResources, queue<Resource*> *outputResources) {
	LOG_ERROR(this, "Unimplemented resource processing (ProcessMulti)");
	return 0;
}

int Module::ProcessingResources() {
	LOG_ERROR(this, "Unimplemented resource processing (ProcessingResources)");
	return 0;
}

void Module::Start() {
}

void Module::Stop() {
}

void Module::Pause() {
}

void Module::Resume() {
}
