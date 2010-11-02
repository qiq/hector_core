/**
 * Test module.
 */
#include <config.h>

#include <string.h>
#include "Dump.h"

using namespace std;

Dump::Dump(ObjectRegistry *objects, ProcessingEngine *engine, const char *id, int threadIndex): Module(objects, engine, id, threadIndex) {
	values = new ObjectValues<Dump>(this);
}

Dump::~Dump() {
	delete values;
}

bool Dump::Init(vector<pair<string, string> > *params) {
	values->InitValues(params);
	return true;
}

Resource *Dump::ProcessSimple(Resource *resource) {
	string *s = resource->toString();
	LOG_DEBUG(*s);
	delete s;
	return resource;
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, ProcessingEngine *engine, const char *id, int threadIndex) {
	return new Dump(objects, engine, id, threadIndex);
}
