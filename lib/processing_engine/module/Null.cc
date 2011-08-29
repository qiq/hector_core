/**
 * Test module.
 */
#include <config.h>

#include <string.h>
#include <queue>
#include "common.h"
#include "Null.h"

using namespace std;

#define MAX_RESOURCES 10000

Null::Null(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	moduleType = INVALID;
	items = 0;

	props = new ObjectProperties<Null>(this);
	props->Add("moduleType", &Null::GetModuleType, &Null::SetModuleType, true);
	props->Add("items", &Null::GetItems);

	typeId = -1;
}

Null::~Null() {
	delete props;
}

char *Null::GetModuleType(const char *name) {
	switch (moduleType) {
	case INPUT:
		return strdup("INPUT");
	case SIMPLE:
		return strdup("SIMPLE");
	case MULTI:
		return strdup("MULTI");
	case OUTPUT:
		return strdup("OUTPUT");
	default:
	case INVALID:
		return strdup("INVALID");
	}
}

void Null::SetModuleType(const char *name, const char *value) {
	if (!strcmp(value, "INPUT"))
		moduleType = INPUT;
	else if (!strcmp(value, "SIMPLE"))
		moduleType = SIMPLE;
	else if (!strcmp(value, "MULTI"))
		moduleType = MULTI;
	else if (!strcmp(value, "OUTPUT"))
		moduleType = OUTPUT;
	else
		LOG_ERROR(this, "Invalid moduleType value: " << value);
}

char *Null::GetItems(const char *name) {
	return int2str(items);
}

bool Null::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params)
		return true;

	if (!props->InitProperties(params))
		return false;

	if (moduleType == INVALID) {
		LOG_ERROR(this, "No moduleType parameter given.");
		return false;
	}

	typeId = Resource::GetRegistry()->NameToId("TestResource");

	return true;
}

Resource *Null::ProcessInputSync(bool sleep) {
	Resource *r = Resource::GetRegistry()->AcquireResource(typeId);
	items++;
	return r;
}

Resource *Null::ProcessSimpleSync(Resource *r) {
	return r;
}

bool Null::ProcessMultiSync(queue<Resource*> *inputResources, queue<Resource*> *outputResources, int *expectResources, int *processingResources) {
	while (inputResources->size() > 0) {
		outputResources->push(inputResources->front());
		inputResources->pop();
	}
	if (expectResources)
		*expectResources = MAX_RESOURCES;
	if (processingResources)
		*processingResources = 0;
	return false;
}

Resource *Null::ProcessOutputSync(Resource *r) {
	return r;
}

// the class factories

extern "C" Module* hector_module_create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new Null(objects, id, threadIndex);
}
