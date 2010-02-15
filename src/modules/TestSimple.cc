/**
 * Test module.
 */
#include <config.h>

#include <string.h>
#include "TestSimple.h"
#include "TestResource.h"

TestSimple::TestSimple(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	foo = NULL;

	char s[1024];

	snprintf(s, sizeof(s), "%d.items", threadIndex);
	getters[s] = &TestSimple::getItems;
	snprintf(s, sizeof(s), "%d.foo", threadIndex);
	getters[s] = &TestSimple::getFoo;
	setters[s] = &TestSimple::setFoo;
	snprintf(s, sizeof(s), "%d.alias", threadIndex);
	getters[s] = &TestSimple::getFoo;
	setters[s] = &TestSimple::setFoo;
	snprintf(s, sizeof(s), "%d.flip_status", threadIndex);
	getters[s] = &TestSimple::getFlipStatus;
	setters[s] = &TestSimple::setFlipStatus;
}

TestSimple::~TestSimple() {
	free(foo);
}

char *TestSimple::getItems() {
	char s[1024];
	snprintf(s, sizeof(s), "%d", items);
	return strdup(s);
}

char *TestSimple::getFoo() {
	return foo ? strdup(foo) : NULL;
}

void TestSimple::setFoo(const char *value) {
	free(foo);
	foo = strdup(value);
}

char *TestSimple::getFlipStatus() {
	return bool2str(flipStatus);
}

void TestSimple::setFlipStatus(const char *value) {
	switch (str2bool(value)) {
	case 0:
		flipStatus = false;
		break;
	case 1:
		flipStatus = true;
		break;
	default:
		MODULE_LOG_ERROR(logger, "Invalid boolean value: " << value);
	}
}

bool TestSimple::Init(vector<pair<string, string> > *params) {
	return true;
}

Resource *TestSimple::Process(Resource *resource) {
	TestResource *tr = dynamic_cast<TestResource*>(resource);
	if (tr) {
		MODULE_LOG_INFO(logger, "Processing TestResource (" << tr->getStr() << ")");
		if (flipStatus)
			tr->setStatus(tr->getStatus() == 0 ? 1 : 0);
		++items;
	}
	return resource;
}

char *TestSimple::getValueSync(const char *name) {
	std::tr1::unordered_map<string, char*(TestSimple::*)()>::iterator iter = getters.find(name);
	if (iter != getters.end())
		return (this->*(iter->second))();
	return NULL;
}

bool TestSimple::setValueSync(const char *name, const char *value) {
	std::tr1::unordered_map<string, void(TestSimple::*)(const char*)>::iterator iter = setters.find(name);
	if (iter != setters.end()) {
		(this->*(iter->second))(value);
		return true;
	}
	return false;
}

vector<string> *TestSimple::listNamesSync() {
	vector<string> *result = new vector<string>();
	for (std::tr1::unordered_map<string, char*(TestSimple::*)()>::iterator iter = getters.begin(); iter != getters.end(); ++iter) {
		result->push_back(iter->first);
	}
	return result;
}

// factory functions

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return new TestSimple(objects, id, threadIndex);
}

extern "C" void destroy(Module* p) {
	delete p;
}
