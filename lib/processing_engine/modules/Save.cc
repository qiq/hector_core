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
#include "ResourceOutputStreamBinary.h"
#include "ResourceOutputStreamText.h"

using namespace std;

Save::Save(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	filename = NULL;
	items = 0;
	overwrite = false;
	saveResourceType = false;
	saveResourceIdStatus = false;
	text = false;
	fd = -1;
	ofs = NULL;
	stream = NULL;

	props = new ObjectProperties<Save>(this);
	props->Add("items", &Save::GetItems);
	props->Add("filename", &Save::GetFilename, &Save::SetFilename, true);
	props->Add("overwrite", &Save::GetOverwrite, &Save::SetOverwrite, true);
	props->Add("saveResourceType", &Save::GetSaveResourceType, &Save::SetSaveResourceType, true);
	props->Add("saveResourceIdStatus", &Save::GetSaveResourceIdStatus, &Save::SetSaveResourceIdStatus, true);
	props->Add("text", &Save::GetText, &Save::SetText);
}

Save::~Save() {
	delete stream;
	if (fd >= 0) {
		flock(fd, LOCK_UN);
		close(fd);
	}
	if (ofs)
		delete ofs;
	free(filename);
	delete props;
}

char *Save::GetItems(const char *name) {
	return int2str(items);
}

char *Save::GetFilename(const char *name) {
	return filename ? strdup(filename) : NULL;
}

bool Save::ReopenFile() {
	delete stream;
	if (fd >= 0) {
		flock(fd, LOCK_UN);
		close(fd);
	}
	if (ofs) {
		delete ofs;
		ofs = NULL;
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
	if (!text) {
		stream = new ResourceOutputStreamBinary(fd);
	} else {
		ofs = new ofstream();
		ofs->open(filename, ofstream::out|ifstream::binary);
		if (ofs->fail()) {
			LOG_ERROR(this, "Cannot open output file " << filename);
			return false;
		}
		stream = new ResourceOutputStreamText(ofs);
	}
	return true;
}

void Save::SetFilename(const char *name, const char *value) {
	free(filename);
	filename = strdup(value);
	if (fd >= 0)
		ReopenFile();
}

char *Save::GetOverwrite(const char *name) {
	return bool2str(overwrite);
}

void Save::SetOverwrite(const char *name, const char *value) {
	overwrite = str2bool(value);
}

char *Save::GetSaveResourceType(const char *name) {
	return bool2str(saveResourceType);
}

void Save::SetSaveResourceType(const char *name, const char *value) {
	saveResourceType = str2bool(value);
}

char *Save::GetSaveResourceIdStatus(const char *name) {
	return bool2str(saveResourceIdStatus);
}

void Save::SetSaveResourceIdStatus(const char *name, const char *value) {
	saveResourceIdStatus = str2bool(value);
}

char *Save::GetText(const char *name) {
	return bool2str(text);
}

void Save::SetText(const char *name, const char *value) {
	text = str2bool(value);
}

bool Save::Init(vector<pair<string, string> > *params) {
	// second stage?
	if (!params)
		return true;

	if (!props->InitProperties(params))
		return false;

	if (!filename) {
		LOG_ERROR(this, "No filename parameter given.");
		return false;
	}

	// open file for the first time
	ReopenFile();

	return true;
}

Resource *Save::ProcessOutputSync(Resource *resource) {
	assert(resource != NULL);
	if (!stream) {
		LOG_ERROR_R(this, resource, "File not opened");
		return resource;
	}
	bool res = Resource::Serialize(resource, *stream, saveResourceType, saveResourceIdStatus);
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
