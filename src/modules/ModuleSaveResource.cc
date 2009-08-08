/**
 * Dummy module.
 */
#include <config.h>

#include "ModuleSaveResource.h"
#include "WebResource.h"

bool ModuleSaveResource::Init(Config *config) {
	return true;
}

void ModuleSaveResource::Process(Resource *resource) {
// FIXME: WTF?
//	WebResource *wb = dynamic_cast<WebResouce*>(resource);
//	if (wb) {
//		fprintf(stderr, "Saving resource (URL: %s)\n", resource->getURL());
//	}
	return;
}

void ModuleSaveResource::createCheckpoint() {
}

const char *ModuleSaveResource::getValue(const char *name) {
	return "abc";
}

bool ModuleSaveResource::setValue(const char *name, const char *value) {
	return false;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id) {
	return (Module*)new ModuleSaveResource(objects, id);
}

extern "C" void destroy(Module* p) {
	delete p;
}

