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
	byteCount = 0;
	fd = -1;
	file = NULL;
	stream = NULL;

	values = new ObjectValues<Load>(this);
	values->AddGetter("items", &Load::GetItems);
	values->AddGetter("maxItems", &Load::GetMaxItems);
	values->AddSetter("maxItems", &Load::SetMaxItems);
	values->AddGetter("filename", &Load::GetFilename);
	values->AddSetter("filename", &Load::SetFilename);
}

Load::~Load() {
	delete stream;
	delete file;
	if (fd >= 0)
		close(fd);
	free(filename);
	delete values;
}

char *Load::GetItems(const char *name) {
	return int2str(items);
}

char *Load::GetMaxItems(const char *name) {
	return int2str(maxItems);
}

void Load::SetMaxItems(const char *name, const char *value) {
	maxItems = str2int(value);
}

char *Load::GetFilename(const char *name) {
	return filename ? strdup(filename) : NULL;
}

void Load::SetFilename(const char *name, const char *value) {
	fileCond.Lock();
	free(filename);
	filename = strdup(value);
	delete stream;
	byteCount = 0;
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		LOG_ERROR(this, "Cannot open file " << filename << ": " << strerror(errno));
		fileCond.Unlock();
		return;
	}
	file = new google::protobuf::io::FileInputStream(fd);
	stream = new google::protobuf::io::CodedInputStream(file);
	fileCond.SignalSend();
	fileCond.Unlock();
}

bool Load::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params)
		return true;

	if (!values->InitValues(params))
		return false;
	if (maxItems)
		LOG_INFO(this, "Going to load " << maxItems << " resources.");
	return true;
}

Resource *Load::ProcessInputSync(bool sleep) {
	Resource *r = NULL;
	if (maxItems && items >= maxItems)
		return NULL;
	while (1) {
		int size = 0;
		r = stream ? Resource::Deserialize(stream, &size) : NULL;
		if (r) {
			byteCount += size;
			break;
		}
		if (!sleep)
			return NULL;
		ObjectUnlock();
		fileCond.Lock();
		fileCond.WaitSend(NULL);
		fileCond.Unlock();
		ObjectLockWrite();
		if (cancel)
			return NULL;
	}
	++items;
	return r;
}

bool Load::SaveCheckpointSync(const char *path) {
	char buffer1[1024];
	snprintf(buffer1, sizeof(buffer1), "%s.%s", path, GetId());
	FILE *fw = fopen(buffer1, "w");
	if (!fw) {
		LOG_ERROR(this, "Cannot open file: " << buffer1);
		return false;
	}
	char buffer2[1024];
	snprintf(buffer2, sizeof(buffer2), "%s\n%llu\n", filename, byteCount);
	if (fwrite(buffer2, strlen(buffer2), 1, fw) != 1) {
		LOG_ERROR(this, "Cannot write data to file: " << buffer1);
		fclose(fw);
		return false;
	}
	fclose(fw);
	return true;
}

bool Load::RestoreCheckpointSync(const char *path) {
	char buffer1[1024];
	snprintf(buffer1, sizeof(buffer1), "%s.%s", path, GetId());
	FILE *fr = fopen(buffer1, "r");
	if (!fr) {
		LOG_ERROR(this, "Cannot open file: " << buffer1);
		return false;
	}
	char buffer2[1024];
	int n = fread(buffer2, 1, 1023, fr);
	fclose(fr);
	if (n == 0) {
		LOG_ERROR(this, "Cannot read data from file: " << buffer1);
		return false;
	}
	buffer2[n] = '\0';
	char fn[1024];
	unsigned long long offset;
	if (sscanf(buffer2, "%s\n%llu\n", fn, &offset) != 2) {
		LOG_ERROR(this, "Cannot parse data: " << buffer1);
		return false;
	}
	SetFilename(NULL, fn);
	if (!stream->Skip(offset)) {
		LOG_ERROR(this, "Cannot seek in file: " << filename);
		return false;
	}
	return true;
}

void Load::StartSync() {
	cancel = false;
}

void Load::StopSync() {
	cancel = true;
	ObjectUnlock();
	fileCond.Lock();
	fileCond.SignalSend();
	fileCond.Unlock();
	ObjectLockWrite();
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new Load(objects, id, threadIndex);
}
