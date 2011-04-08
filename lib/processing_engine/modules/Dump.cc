/**
 * Test module.
 */
#include <config.h>

#include <string.h>
#include "Dump.h"

using namespace std;

Dump::Dump(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	props = new ObjectProperties<Dump>(this);
}

Dump::~Dump() {
	delete props;
}

bool Dump::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params)
		return true;

	props->InitProperties(params);
	return true;
}

Resource *Dump::ProcessSimpleSync(Resource *resource) {
	string s = resource->ToString();
	LOG_DEBUG(this, s);
	return resource;
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return new Dump(objects, id, threadIndex);
}
