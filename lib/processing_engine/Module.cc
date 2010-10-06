//
#include <config.h>

#include "common.h"
#include "Module.h"

using namespace std;

bool Module::Init(vector<pair<string, string> > *args) {
	LOG_ERROR("Unimplemented module " << getId() << " (Init)");
	return false;
}

Module::Type Module::getType() {
	LOG_ERROR("Unimplemented module " << getId() << " (getType)");
	return Module::INVALID;
}

Resource *Module::ProcessInput(bool sleep) {
	LOG_ERROR("Unimplemented resource processing " << getId() << " (ProcessInput)");
	return NULL;
}

void Module::ProcessOutput(Resource *resource) {
	LOG_ERROR("Unimplemented resource processing " << getId() << " (ProcessOutput)");
}

Resource *Module::ProcessSimple(Resource *resource) {
	LOG_ERROR("Unimplemented resource processing " << getId() << " (ProcessSimple)");
	return NULL;
}

int Module::ProcessMulti(queue<Resource*> *inputResources, queue<Resource*> *outputResources) {
	LOG_ERROR("Unimplemented resource processing " << getId() << " (ProcessMulti)");
	return 0;
}

int Module::ProcessingResources() {
	LOG_ERROR("Unimplemented resource processing " << getId() << " (ProcessingResources");
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
