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
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "common.h"
#include "ProtobufResource.h"
#include "Resources.h"
#include "Save.h"
#include "TestResource.h"

using namespace std;

Save::Save(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	filename = NULL;
	fd = -1;
	items = 0;
	values = new ObjectValues<Save>(this);

	values->addGetter("items", &Save::getItems);
	values->addGetter("filename", &Save::getFilename);
	values->addSetter("filename", &Save::setFilename);
}

Save::~Save() {
	if (!stream->Close())
		LOG_ERROR(logger, "Error closing file: " << filename << " (" << strerror(stream->GetErrno()) << ").")
	delete stream;
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
	if (!values->InitValues(params))
		return false;

	if (!filename) {
		LOG_ERROR(logger, "No filename parameter given.");
		return false;
	}

	if ((fd = open(filename, O_WRONLY|O_APPEND|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0) {
		LOG_ERROR(logger, "Cannot open file " << filename << ": " << strerror(errno));
		return false;
	}

	stream = new google::protobuf::io::FileOutputStream(fd);

	return true;
}

bool Save::WriteToFile(const void *data, int size) {
	while (size > 0) {
		ssize_t wr = write(fd, data, size);
		if (wr > 0) {
			size -= wr;
		} else {
			ObjectLockRead();
			LOG_ERROR(logger, "Cannot read from file: " << filename << " (" << strerror(errno) << "), giving up.")
			ObjectUnlock();
			return false;
		}
	}
	return true;
}

Resource *Save::Process(Resource *resource) {
	assert(resource != NULL);
	ProtobufResource *pr = dynamic_cast<ProtobufResource*>(resource);
	uint32_t size;
	uint8_t typeId = resource->getTypeId();
	if (pr) {
		if (!WriteToFile(&size, sizeof(size)))
			return NULL;
		if (!WriteToFile(&typeId, sizeof(typeId)))
			return NULL;
		if (!pr->Serialize(stream))
			return NULL;
	} else {
		string *serial = resource->Serialize();
		if (!serial)
			return NULL;
		size = serial->length();
		if (!WriteToFile(&size, sizeof(size)))
			return NULL;
		if (!WriteToFile(&typeId, sizeof(typeId)))
			return NULL;
		if (!WriteToFile(serial->c_str(), size))
			return NULL;
	}

	delete resource;

	ObjectLockWrite();
	++items;
	ObjectUnlock();

	return NULL;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new Save(objects, id, threadIndex);
}
