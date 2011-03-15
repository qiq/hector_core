//
#include <config.h>

#include "common.h"
#include "Module.h"

using namespace std;

bool Module::Init(vector<pair<string, string> > *args) {
	LOG_ERROR(this, "Unimplemented module " << GetId() << " (Init)");
	return false;
}

Module::Type Module::GetType() {
	LOG_ERROR(this, "Unimplemented module " << GetId() << " (GetType)");
	return Module::INVALID;
}

Resource *Module::ProcessInputSync(bool sleep) {
	LOG_ERROR(this, "Unimplemented resource processing (ProcessInput)");
	return NULL;
}

Resource *Module::ProcessOutputSync(Resource *resource) {
	LOG_ERROR(this, "Unimplemented resource processing (ProcessOutput)");
	return NULL;
}

Resource *Module::ProcessSimpleSync(Resource *resource) {
	LOG_ERROR(this, "Unimplemented resource processing (ProcessSimple)");
	return NULL;
}

int Module::ProcessMultiSync(queue<Resource*> *inputResources, queue<Resource*> *outputResources, int *expectingResources) {
	LOG_ERROR(this, "Unimplemented resource processing (ProcessMulti)");
	return 0;
}

void Module::StartSync() {
}

void Module::StopSync() {
}

void Module::PauseSync() {
}

void Module::ResumeSync() {
}
