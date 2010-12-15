/**
 * Test module.
 */
#include <config.h>

#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common.h"
#include "ProtobufResource.h"
#include "Save.h"
#include "TestResource.h"

using namespace std;

Save::Save(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	filename = NULL;
	items = 0;
	fd = -1;
	file = NULL;
	stream = NULL;

	values = new ObjectValues<Save>(this);
	values->addGetter("items", &Save::getItems);
	values->addGetter("filename", &Save::getFilename);
	values->addSetter("filename", &Save::setFilename, true);
}

Save::~Save() {
	delete stream;
	delete file;
	close(fd);
	free(filename);
	delete values;
}

char *Save::getItems(const char *name) {
	return int2str(items);
}

char *Save::getFilename(const char *name) {
	return filename ? strdup(filename) : NULL;
}

void Save::setFilename(const char *name, const char *value) {
	free(filename);
	filename = strdup(value);
}

bool Save::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params)
		return true;

	if (!values->InitValues(params))
		return false;

	if (!filename) {
		LOG_ERROR(this, "No filename parameter given.");
		return false;
	}

	fd = open(filename, O_WRONLY|O_APPEND|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd < 0) {
		LOG_ERROR(this, "Cannot open file " << filename << ": " << strerror(errno));
		return false;
	}
	file = new google::protobuf::io::FileOutputStream(fd);
	stream = new google::protobuf::io::CodedOutputStream(file);

	return true;
}

Resource *Save::ProcessOutput(Resource *resource) {
	assert(resource != NULL);
	ObjectLockWrite();
	if (!stream) {
		ObjectUnlock();
		LOG_ERROR_R(this, resource, "File not opened");
		return resource;
	}
	bool res = Resource::Serialize(resource, stream);
	if (res)
		++items;
	ObjectUnlock();
	if (!res)
		LOG_ERROR_R(this, resource, "Cannot serialize resource");
	return resource;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new Save(objects, id, threadIndex);
}
