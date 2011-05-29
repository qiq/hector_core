/**
 * Test module.
 */
#include <config.h>

#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common.h"
#include "Load.h"
#include "MarkerResource.h"
#include "ResourceInputStreamBinary.h"
#include "ResourceInputStreamText.h"

using namespace std;

Load::Load(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	items = 0;
	maxItems = 0;
	filename = NULL;
	wait = false;
	resourceType = 0;
	resourceTypeName = strdup("");
	text = false;
	mark = false;
	markEmmited = false;
	byteCount = 0;
	fd = -1;
	ifs = NULL;
	stream = NULL;

	props = new ObjectProperties<Load>(this);
	props->Add("items", &Load::GetItems);
	props->Add("maxItems", &Load::GetMaxItems, &Load::SetMaxItems);
	props->Add("filename", &Load::GetFilename, &Load::SetFilename);
	props->Add("wait", &Load::GetWait, &Load::SetWait);
	props->Add("resourceType", &Load::GetResourceType, &Load::SetResourceType);
	props->Add("mark", &Load::GetMark, &Load::SetMark);
	props->Add("text", &Load::GetText, &Load::SetText);
}

Load::~Load() {
	delete stream;
	if (fd >= 0) {
		flock(fd, LOCK_UN);
		close(fd);
	}
	delete ifs;
	free(filename);
	free(resourceTypeName);
	delete props;
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

bool Load::ReopenFile() {
	fileCond.Lock();
	delete stream;
	stream = NULL;
	if (fd >= 0) {
		flock(fd, LOCK_UN);
		close(fd);
	}
	if (ifs) {
		delete ifs;
		ifs = NULL;
	}
	markEmmited = false;
	byteCount = 0;
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		LOG_ERROR(this, "Cannot open file " << filename << ": " << strerror(errno));
		fileCond.Unlock();
		return false;
	}
	if (flock(fd, LOCK_SH) < 0) {
		LOG_ERROR(this, "Cannot lock file " << filename << ": " << strerror(errno));
		fileCond.Unlock();
		return false;
	}

	if (!text) {
		stream = new ResourceInputStreamBinary(fd);
	} else {
		ifs = new ifstream();
		ifs->open(filename, ifstream::in|ifstream::binary);
		if (ifs->fail()) {
			LOG_ERROR(this, "Cannot open input file " << filename);
			return false;
		}
		stream = new ResourceInputStreamText(ifs);
	}

	fileCond.SignalSend();
	fileCond.Unlock();
	return true;
}

void Load::SetFilename(const char *name, const char *value) {
	free(filename);
	filename = strdup(value);
	if (fd >= 0)
		ReopenFile();
}

char *Load::GetWait(const char *name) {
	return bool2str(wait);
}

void Load::SetWait(const char *name, const char *value) {
	wait = str2bool(value);
}

char *Load::GetResourceType(const char *name) {
	return strdup(resourceTypeName);
}

void Load::SetResourceType(const char *name, const char *value) {
	int type = 0;
	if (strcmp(value, "")) {
		type = Resource::GetRegistry()->NameToId(value);
		if (type <= 0) {
			LOG_ERROR(this, "Invalid resourceType: " << value);
			return;
		}
	}
	free(resourceTypeName);
	resourceTypeName = strdup(value);
	resourceType = type;
}

char *Load::GetMark(const char *name) {
	return bool2str(mark);
}

void Load::SetMark(const char *name, const char *value) {
	mark = str2bool(value);
}

char *Load::GetText(const char *name) {
	return bool2str(text);
}

void Load::SetText(const char *name, const char *value) {
	text = str2bool(value);
}

bool Load::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params)
		return true;

	if (!props->InitProperties(params))
		return false;

	if (mark) {
		markerResourceTypeId = Resource::GetRegistry()->NameToId("MarkerResource");
		if (markerResourceTypeId <= 0) {
			LOG_ERROR(this, "Unknown resource type: MarkerResource");
			return false;
		}
	}

	// open file for the first time
	ReopenFile();

	if (maxItems)
		LOG_INFO(this, "Going to load " << maxItems << " resources.");
	return true;
}

Resource *Load::ProcessInputSync(bool sleep) {
	Resource *r = NULL;
	if (maxItems && items >= maxItems) {
		if (mark && !markEmmited) {
			MarkerResource *mr = static_cast<MarkerResource*>(Resource::GetRegistry()->AcquireResource(markerResourceTypeId));
			mr->SetMark(1);
			markEmmited = true;
			return mr;
		}
		return NULL;
	}

	while (1) {
		int size = 0;
		r = stream ? Resource::DeserializeResource(*stream, resourceType, &size) : NULL;
		if (r) {
			byteCount += size;
			break;
		}
		if (!sleep || !wait) {
			if (mark && !markEmmited) {
				MarkerResource *mr = static_cast<MarkerResource*>(Resource::GetRegistry()->AcquireResource(markerResourceTypeId));
				mr->SetMark(1);
				markEmmited = true;
				return mr;
			}
			LOG_INFO(this, "Resources read: " << items);
			return NULL;
		}
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

extern "C" Module* hector_module_create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new Load(objects, id, threadIndex);
}
