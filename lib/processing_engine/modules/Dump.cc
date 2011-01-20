/**
 * Test module.
 */
#include <config.h>

#include <string.h>
#include "Dump.h"

using namespace std;

Dump::Dump(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	values = new ObjectValues<Dump>(this);
}

Dump::~Dump() {
	delete values;
}

bool Dump::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params)
		return true;

	values->InitValues(params);
	return true;
}

Resource *Dump::ProcessSimpleSync(Resource *resource) {
	string s = resource->toString();
	LOG_DEBUG(this, s);
	return resource;
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return new Dump(objects, id, threadIndex);
}
