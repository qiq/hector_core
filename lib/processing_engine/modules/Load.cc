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
#include "Load.h"
#include "TestResource.h"

using namespace std;

Load::Load(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	maxItems = 0;
	filename = NULL;
	fd = -1;
	stream = NULL;

	values = new ObjectValues<Load>(this);
	values->addGetter("items", &Load::getItems);
	values->addGetter("maxItems", &Load::getMaxItems);
	values->addSetter("maxItems", &Load::setMaxItems);
	values->addGetter("filename", &Load::getFilename);
	values->addSetter("filename", &Load::setFilename);
}

Load::~Load() {
	if (stream && !stream->Close())
		LOG_ERROR("Error closing file: " << filename << " (" << strerror(stream->GetErrno()) << ").")
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
	fileCond.Lock();
	free(filename);
	filename = strdup(value);
	if (stream && !stream->Close())
		LOG_ERROR("Error closing file: " << filename << " (" << strerror(stream->GetErrno()) << ").")
	delete stream;
	if (fd >= 0)
		close(fd);
	if ((fd = open(filename, O_RDONLY)) >= 0)
		stream = new google::protobuf::io::FileInputStream(fd);
	else
		LOG_ERROR("Cannot open file " << filename << ": " << strerror(errno));
	fileCond.SignalSend();
	fileCond.Unlock();
}

bool Load::Init(vector<pair<string, string> > *params) {
	if (!values->InitValues(params))
		return false;
	if (maxItems)
		LOG_INFO("Going to load " << maxItems << " resources.");
	return true;
}

bool Load::ReadFromFile(char *data, int size, bool sleep) {
	ObjectLockRead();
	int f = fd;
	ObjectUnlock();
	int offset = 0;
	while (size > 0) {
		int count = ReadBytes(fd, data+offset, size);
		if (count < 0) {
			if (count < 0) {
				ObjectLockRead();
				LOG_ERROR("Cannot read from file: " << filename << " (" << strerror(errno) << "), giving up.")
				ObjectUnlock();
				return false;
			}
		} else if (count == 0) {
			// no more data to be read from this file
			if (!sleep)
				return false;
			fileCond.Lock();
			fileCond.WaitSend(NULL);
			fileCond.Unlock();
			ObjectLockRead();
			bool c = cancel;
			ObjectUnlock();
			if (c)
				return false;
		} else {
			size -= count;
			offset += count;
		}
	}
	return true;
}

Resource *Load::ProcessInput(bool sleep) {
	ObjectLockRead();
	int i = items;
	ObjectUnlock();
	if (maxItems && i >= maxItems)
		return NULL;
	char buffer[5];
	if (!ReadFromFile(buffer, 5, sleep))
		return NULL;
	uint32_t size = *(uint32_t*)buffer;
	uint8_t typeId = *(uint8_t*)(buffer+4);

	Resource *r = Resource::CreateResource(typeId);
	ProtobufResource *pr = dynamic_cast<ProtobufResource*>(r);
	if (pr) {
		ObjectLockRead();
		bool result = pr->Deserialize(stream, (int)size);
		ObjectUnlock();
		if (!result)
			return NULL;
	} else {
		char *data = (char*)malloc(size);
		if (!ReadFromFile(data, size, false)) {
			free(data);
			delete r;
			return NULL;
		}
		if (!r->Deserialize(data, size)) {
			free(data);
			delete r;
			return NULL;
		}
		free(data);
	}
	ObjectLockWrite();
	++items;
	ObjectUnlock();
	return r;
}

bool Load::SaveCheckpointSync(const char *path) {
	char buffer1[1024];
	snprintf(buffer1, sizeof(buffer1), "%s.%s", path, getId());
	FILE *fw = fopen(buffer1, "w");
	if (!fw) {
		LOG_ERROR("Cannot open file: " << buffer1);
		return false;
	}
	char buffer2[1024];
	off_t offset = lseek(fd, 0, SEEK_CUR);
	snprintf(buffer2, sizeof(buffer2), "%s\n%llu\n", filename, (unsigned long long)offset);
	if (fwrite(buffer2, strlen(buffer2), 1, fw) != 1) {
		LOG_ERROR("Cannot write data to file: " << buffer1);
		fclose(fw);
		return false;
	}
	fclose(fw);
	return true;
}

bool Load::RestoreCheckpointSync(const char *path) {
	char buffer1[1024];
	snprintf(buffer1, sizeof(buffer1), "%s.%s", path, getId());
	FILE *fr = fopen(buffer1, "r");
	if (!fr) {
		LOG_ERROR("Cannot open file: " << buffer1);
		return false;
	}
	char buffer2[1024];
	int n = fread(buffer2, 1, 1023, fr);
	fclose(fr);
	if (n == 0) {
		LOG_ERROR("Cannot read data from file: " << buffer1);
		return false;
	}
	buffer2[n] = '\0';
	char fn[1024];
	unsigned long long offset;
	if (sscanf(buffer2, "%s\n%llu\n", fn, &offset) != 2) {
		LOG_ERROR("Cannot parse data: " << buffer1);
		return false;
	}
	setFilename(NULL, fn);
	if (lseek(fd, (off_t)offset, SEEK_SET) != offset) {
		LOG_ERROR("Cannot seek in file: " << filename);
		return false;
	}
	return true;
}

void Load::Start() {
	ObjectLockWrite();
	cancel = false;
	ObjectUnlock();
}

void Load::Stop() {
	ObjectLockWrite();
	cancel = true;
	ObjectUnlock();
	fileCond.Lock();
	fileCond.SignalSend();
	fileCond.Unlock();
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new Load(objects, id, threadIndex);
}
