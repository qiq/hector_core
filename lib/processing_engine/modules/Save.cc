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
#include "Save.h"

using namespace std;

Save::Save(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	filename = NULL;
	items = 0;
	overwrite = false;
	saveType = false;
	saveIdStatus = false;
	fd = -1;
	stream = NULL;

	values = new ObjectValues<Save>(this);
	values->Add("items", &Save::GetItems);
	values->Add("filename", &Save::GetFilename, &Save::SetFilename, true);
	values->Add("overwrite", &Save::GetOverwrite, &Save::SetOverwrite, true);
	values->Add("saveType", &Save::GetSaveType, &Save::SetSaveType, true);
	values->Add("saveIdStatus", &Save::GetSaveIdStatus, &Save::SetSaveIdStatus, true);
}

Save::~Save() {
	delete stream;
	if (fd >= 0) {
		flock(fd, LOCK_UN);
		close(fd);
	}
	free(filename);
	delete values;
}

char *Save::GetItems(const char *name) {
	return int2str(items);
}

char *Save::GetFilename(const char *name) {
	return filename ? strdup(filename) : NULL;
}

void Save::SetFilename(const char *name, const char *value) {
	free(filename);
	filename = strdup(value);
	delete stream;
	if (fd >= 0) {
		flock(fd, LOCK_UN);
		close(fd);
	}
	int flags = O_WRONLY|O_CREAT;
	if (!overwrite)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(filename, flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd < 0) {
		LOG_ERROR(this, "Cannot open file " << filename << ": " << strerror(errno));
		return;
	}
	if (flock(fd, LOCK_EX) < 0) {
		LOG_ERROR(this, "Cannot lock file " << filename << ": " << strerror(errno));
		return;
	}
}

char *Save::GetOverwrite(const char *name) {
	return bool2str(overwrite);
}

void Save::SetOverwrite(const char *name, const char *value) {
	overwrite = str2bool(value);
}

char *Save::GetSaveType(const char *name) {
	return bool2str(saveType);
}

void Save::SetSaveType(const char *name, const char *value) {
	saveType = str2bool(value);
}

char *Save::GetSaveIdStatus(const char *name) {
	return bool2str(saveIdStatus);
}

void Save::SetSaveIdStatus(const char *name, const char *value) {
	saveIdStatus = str2bool(value);
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

	int flags = O_WRONLY|O_CREAT;
	if (!overwrite)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(filename, flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd < 0) {
		LOG_ERROR(this, "Cannot open file " << filename << ": " << strerror(errno));
		return false;
	}
	if (flock(fd, LOCK_EX) < 0) {
		LOG_ERROR(this, "Cannot lock file " << filename << ": " << strerror(errno));
		return false;
	}
	stream = new ResourceOutputStream(fd);

	return true;
}

Resource *Save::ProcessOutputSync(Resource *resource) {
	assert(resource != NULL);
	if (!stream) {
		LOG_ERROR_R(this, resource, "File not opened");
		return resource;
	}
	bool res = Resource::Serialize(resource, *stream, saveType, saveIdStatus);
	if (res)
		++items;
	if (!res)
		LOG_ERROR_R(this, resource, "Cannot serialize resource");
	return resource;
}

// the class factories

extern "C" Module* create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new Save(objects, id, threadIndex);
}
