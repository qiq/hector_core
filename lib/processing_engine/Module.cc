//
#include <config.h>

#include "common.h"
#include "Module.h"

using namespace std;

log4cxx::LoggerPtr Module::logger(log4cxx::Logger::getLogger("lib.processing_engine.Module"));

bool Module::Init(vector<pair<string, string> > *args) {
	LOG_ERROR(logger, "Unimplemented module " << getId() << " (Init)");
	return false;
}

Module::Type Module::getType() {
	LOG_ERROR(logger, "Unimplemented module " << getId() << " (getType)");
	return Module::INVALID;
}

Resource *Module::Process(Resource *resource) {
	LOG_ERROR(logger, "Unimplemented resource processing " << getId() << " (Process)");
	return NULL;
}

int Module::ProcessMulti(queue<Resource*> *inputResources, queue<Resource*> *outputResources) {
	LOG_ERROR(logger, "Unimplemented resource processing " << getId() << " (ProcessMulti)");
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
