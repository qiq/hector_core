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

Resource *Module::Process(Resource *resource) {
	LOG_ERROR("Unimplemented resource processing " << getId() << " (Process)");
	return NULL;
}

int Module::ProcessMulti(queue<Resource*> *inputResources, queue<Resource*> *outputResources) {
	LOG_ERROR("Unimplemented resource processing " << getId() << " (ProcessMulti)");
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
