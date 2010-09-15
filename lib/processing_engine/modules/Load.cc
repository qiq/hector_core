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
#include "Load.h"
#include "TestResource.h"

using namespace std;

Load::Load(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	maxItems = 0;
	filename = NULL;
	fd = -1;
	values = new ObjectValues<Load>(this);

	values->addGetter("items", &Load::getItems);
	values->addGetter("maxItems", &Load::getMaxItems);
	values->addSetter("maxItems", &Load::setMaxItems);
	values->addGetter("filename", &Load::getFilename);
	values->addSetter("filename", &Load::setFilename);
}

Load::~Load() {
	if (!stream->Close())
		LOG_ERROR(logger, "Error closing file: " << filename << " (" << strerror(stream->GetErrno()) << ").")
	delete stream;
	free(filename);
	delete values;
}

char *Load::getItems(const char *name) {
	return int2str(items);
}

char *Load::getMaxItems(const char *name) {
	return int2str(maxItems);
}

void Load::setMaxItems(const char *name, const char *value) {
	maxItems = str2int(value);
}

char *Load::getFilename(const char *name) {
	return filename ? strdup(filename) : NULL;
}

void Load::setFilename(const char *name, const char *value) {
	free(filename);
	filename = strdup(value);
}

bool Load::Init(vector<pair<string, string> > *params) {
	if (!values->InitValues(params))
		return false;
	if (maxItems)
		LOG_INFO(logger, "Going to load " << maxItems << " resources.");

	if (!filename) {
		LOG_ERROR(logger, "No filename parameter given.");
		return false;
	}

	if ((fd = open(filename, O_RDONLY)) < 0) {
		LOG_ERROR(logger, "Cannot open file " << filename << ": " << strerror(errno));
		return false;
	}

	stream = new google::protobuf::io::FileInputStream(fd);

	return true;
}

bool Load::ReadFromFile(void *data, int size) {
	while (size > 0) {
		ssize_t rd = read(fd, data, size);
		if (rd > 0) {
			size -= rd;
		} else {
			ObjectLockRead();
			if (rd < 0)
				LOG_ERROR(logger, "Cannot read from file: " << filename << " (" << strerror(errno) << "), giving up.")
			else
				LOG_INFO(logger, "Input file: " << filename << " read, finishing.");
			ObjectUnlock();
			return false;
		}
	}
	return true;
}

Resource *Load::Process(Resource *resource) {
	ObjectLockRead();
	int i = items;
	int mi = maxItems;
	ObjectUnlock();
	if (mi && i >= mi)
		return NULL;
	assert(resource == NULL);
	uint32_t size;
	uint8_t typeId;
	if (!ReadFromFile(&size, sizeof(size)))
		return NULL;
	if (!ReadFromFile(&typeId, sizeof(typeId)))
		return NULL;

	Resource *r = Resources::CreateResource(typeId);
	ProtobufResource *pr = dynamic_cast<ProtobufResource*>(r);
	if (pr) {
		if (!pr->Deserialize(stream, (int)size))
			return NULL;
	} else {
		void *data = malloc(size);
		if (!ReadFromFile(data, size)) {
			free(data);
			delete r;
			return NULL;
		}
		string *s = new string((char*)data, size);
		if (!r->Deserialize(s)) {
			delete s;
			free(data);
			delete r;
			return NULL;
		}
		delete s;
		free(data);
	}
	ObjectLockWrite();
	++items;
	ObjectUnlock();
	return r;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new Load(objects, id, threadIndex);
}
