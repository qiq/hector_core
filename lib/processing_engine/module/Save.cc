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

#define MAX_RESOURCES 10000
#define DEFAULT_TIMETICK 100*1000

Save::Save(ObjectRegistry *objects, const char *id, int threadIndex): Module(objects, id, threadIndex) {
	isOutputModuleType = true;
	items = 0;
	filename = NULL;
	overwrite = true;
	saveResourceType = true;
	saveResourceIdStatus = false;
	text = false;
	compress = false;
	resourceTypeFilter = "";
	maxItemsPerFile = 0;
	maxSizePerFile = 0;
	timeTick = DEFAULT_TIMETICK;

	props = new ObjectProperties<Save>(this);
	props->Add("moduleType", &Save::GetModuleType, &Save::SetModuleType, true);
	props->Add("items", &Save::GetItems);
	props->Add("filename", &Save::GetFilename, &Save::SetFilename, true);
	props->Add("overwrite", &Save::GetOverwrite, &Save::SetOverwrite, true);
	props->Add("saveResourceType", &Save::GetSaveResourceType, &Save::SetSaveResourceType, true);
	props->Add("saveResourceIdStatus", &Save::GetSaveResourceIdStatus, &Save::SetSaveResourceIdStatus, true);
	props->Add("text", &Save::GetText, &Save::SetText, true);
	props->Add("compress", &Save::GetCompress, &Save::SetCompress, true);
	props->Add("resourceTypeFilter", &Save::GetResourceTypesFilter, &Save::SetResourceTypesFilter);
	props->Add("maxItemsPerFile", &Save::GetMaxItemsPerFile, &Save::SetMaxItemsPerFile, true);
	props->Add("maxSizePerFile", &Save::GetMaxSizePerFile, &Save::SetMaxSizePerFile, true);
	props->Add("timeTick", &Save::GetTimeTick, &Save::SetTimeTick);

	fd = -1;
	ofs = NULL;
	stream = NULL;
	fileId = 1;
	savedItems = 0;
	savedSize = 0;
}

Save::~Save() {
	delete stream;
	if (fd >= 0) {
		flock(fd, LOCK_UN);
		close(fd);
	}
	delete ofs;
	free(filename);
	delete props;
}

char *Save::GetModuleType(const char *name) {
	return isOutputModuleType ? strdup("OUTPUT") : strdup("MULTI");
}

void Save::SetModuleType(const char *name, const char *value) {
	if (!strcmp(value, "OUTPUT"))
		isOutputModuleType = true;
	else if (!strcmp(value, "MULTI"))
		isOutputModuleType = false;
	else
		LOG_ERROR(this, "Invalid moduleType value: " << value);
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
	string s = filename;
	if (maxItemsPerFile > 0 || maxSizePerFile > 0) {
		char id[20];
		snprintf(id, sizeof(id), ".%d", fileId);
		s += id;
	}
	fd = open(s.c_str(), flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd < 0) {
		LOG_ERROR(this, "Cannot open file " << filename << ": " << strerror(errno));
		return false;
	}
	if (flock(fd, LOCK_EX) < 0) {
		LOG_ERROR(this, "Cannot lock file " << filename << ": " << strerror(errno));
		return false;
	}
	if (!text) {
		stream = new ResourceOutputStreamBinary(fd, compress);
	} else {
		ofs = new ofstream();
		ofs->open(filename, ofstream::out|ifstream::binary);
		if (ofs->fail()) {
			LOG_ERROR(this, "Cannot open output file " << filename);
			return false;
		}
		stream = new ResourceOutputStreamText(ofs);
	}
	LOG_DEBUG(this, "Writing to: " << s);
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

char *Save::GetCompress(const char *name) {
	return bool2str(compress);
}

void Save::SetCompress(const char *name, const char *value) {
	compress = str2bool(value);
}

char *Save::GetResourceTypesFilter(const char *name) {
	return strdup(resourceTypeFilter.c_str());
}

void Save::SetResourceTypesFilter(const char *name, const char *value) {
	resourceTypeFilter = value;
	vector<string> v;
	splitOnWs(v, resourceTypeFilter);
	filter.clear();
	for (vector<string>::iterator iter = v.begin(); iter != v.end(); ++iter) {
		int typeId = Resource::GetRegistry()->NameToId(iter->c_str());
		if (typeId >= 0)
			filter.insert(typeId);
		else
			LOG_ERROR(this, "Unknown resource type: " << *iter);
	}
}

char *Save::GetMaxItemsPerFile(const char *name) {
	return int2str(maxItemsPerFile);
}

void Save::SetMaxItemsPerFile(const char *name, const char *value) {
	maxItemsPerFile = str2int(value);
}

char *Save::GetMaxSizePerFile(const char *name) {
	return long2str(maxSizePerFile);
}

void Save::SetMaxSizePerFile(const char *name, const char *value) {
	maxSizePerFile = str2long(value);
}

char *Save::GetTimeTick(const char *name) {
	return int2str(timeTick);
}

void Save::SetTimeTick(const char *name, const char *value) {
	timeTick = str2int(value);
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
	int64_t size = 0;
	if (maxSizePerFile > 0)
		size = resource->GetSize();
	if ((maxItemsPerFile > 0 && savedItems == maxItemsPerFile) || (maxSizePerFile > 0 && savedSize + size > maxSizePerFile)) {
		// update filename
		fileId++;
		// open new file
		ReopenFile();
		savedItems = 0;
		savedSize = 0;
	}
	bool res = Resource::SerializeResource(resource, *stream, saveResourceType, saveResourceIdStatus);
	if (res) {
		items++;
		savedItems++;
		savedSize += size;
	} else {
		LOG_ERROR_R(this, resource, "Cannot serialize resource");
	}
	return resource;
}

bool Save::ProcessMultiSync(queue<Resource*> *inputResources, queue<Resource*> *outputResources, int *expectResources, int *processingResources) {
	while (inputResources->size() > 0) {
		// verbatim copy of resources from intput to output
		Resource *r = inputResources->front();
		uint32_t currentTime = time(NULL);
		int resourcesProcessed = 0;
		if (filter.size() == 0 || filter.find(r->GetTypeId()) != filter.end()) {
			if (!stream) {
				LOG_ERROR_R(this, r, "File not opened");
			} else {
				if (Resource::SerializeResource(r, *stream, saveResourceType, saveResourceIdStatus)) {
					++items;
					if (maxItemsPerFile > 0 && items % maxItemsPerFile == 0) {
						// update filename
						fileId++;
						// open new file
						ReopenFile();
					}
				} else {
					LOG_ERROR_R(this, r, "Cannot serialize resource");
				}
			}
			r->SetFlag(Resource::DELETED);
		}
		outputResources->push(r);
		inputResources->pop();

		// check timeout every 1000 SiteResources read
		if (++resourcesProcessed % 1000 == 0 && ((int)(time(NULL)-currentTime) > timeTick))
			break;
	}

	if (expectResources)
		*expectResources = MAX_RESOURCES;
	if (processingResources)
		*processingResources = 0;
	return false;
}

// the class factories

extern "C" Module* hector_module_create(ObjectRegistry *objects, const char *id, int threadIndex) {
	return (Module*)new Save(objects, id, threadIndex);
}
