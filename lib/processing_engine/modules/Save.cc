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
	values->addSetter("filename", &Save::setFilename, true);
}

Save::~Save() {
	if (!stream->Close())
		LOG_ERROR(this, "Error closing file: " << filename << " (" << strerror(stream->GetErrno()) << ").")
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
	// second stage?
	if (!params)
		return true;

	if (!values->InitValues(params))
		return false;

	if (!filename) {
		LOG_ERROR(this, "No filename parameter given.");
		return false;
	}

	if ((fd = open(filename, O_WRONLY|O_APPEND|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0) {
		LOG_ERROR(this, "Cannot open file " << filename << ": " << strerror(errno));
		return false;
	}

	stream = new google::protobuf::io::FileOutputStream(fd);

	return true;
}

bool Save::WriteToFile(const char *data, int size) {
	ObjectLockRead();
	int f = fd;
	ObjectUnlock();
	if (WriteBytes(f, data, size) != size) {
		ObjectLockRead();
		LOG_ERROR(this, "Cannot write to file: " << filename << " (" << strerror(errno) << "), giving up.")
		ObjectUnlock();
		return false;
	}
	return true;
}

Resource *Save::ProcessOutput(Resource *resource) {
	assert(resource != NULL);
	ProtobufResource *pr = dynamic_cast<ProtobufResource*>(resource);
	char buffer[5];
	*(uint8_t*)(buffer+4) = resource->getTypeId();
	if (pr) {
		*(uint32_t*)buffer = pr->getSerializedSize();
		if (!WriteToFile(buffer, 5))
			return resource;
		if (!pr->SerializeWithCachedSizes(stream))
			return resource;
	} else {
		string *serial = resource->Serialize();
		if (!serial)
			return resource;
		*(uint32_t*)buffer = serial->length();
		if (!WriteToFile(buffer, 5))
			return resource;
		if (!WriteToFile(serial->data(), *(uint32_t*)buffer))
			return resource;
		delete serial;
	}

	ObjectLockWrite();
	++items;
	ObjectUnlock();
	return resource;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new Save(objects, id, threadIndex);
}
