/**
Load.la, input, native
Load Resources from the file.

Dependencies: protobuf

Parameters:
items		r/o	Total items processed
maxItems	r/w	Number of items to load
filename	r/w	File to load. Change it to process another file (and set wait)
wait		r/w	Wait for another file when the current one is exhausted?
resourceType	r/w	Resource type to load (we suppose there is only one resource type in the file)
text		init	Whether to read/write binary or text format (mainly for debugging/testing)
*/

#ifndef _LIB_PROCESSING_ENGINE_MODULES_LOAD_H_
#define _LIB_PROCESSING_ENGINE_MODULES_LOAD_H_

#include <config.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include "CondLock.h"
#include "Module.h"
#include "ObjectProperties.h"
#include "Resource.h"
#include "ResourceInputStream.h"

class Load : public Module {
public:
	Load(ObjectRegistry *objects, const char *id, int threadIndex);
	~Load();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type GetType();
	Resource *ProcessInputSync(bool sleep);
	bool SaveCheckpointSync(const char *path);
	bool RestoreCheckpointSync(const char *path);

	void StartSync();
	void StopSync();

private:
	int items;
	int maxItems;
	char *filename;
	bool wait;
	int resourceType;
	char *resourceTypeName;
	bool text;
	bool mark;

	char *GetItems(const char *name);
	char *GetMaxItems(const char *name);
	void SetMaxItems(const char *name, const char *value);
	char *GetFilename(const char *name);
	void SetFilename(const char *name, const char *value);
	char *GetWait(const char *name);
	void SetWait(const char *name, const char *value);
	char *GetResourceType(const char *name);
	void SetResourceType(const char *name, const char *value);
	char *GetMark(const char *name);
	void SetMark(const char *name, const char *value);
	char *GetText(const char *name);
	void SetText(const char *name, const char *value);

	ObjectProperties<Load> *props;
	char *GetPropertySync(const char *name);
	bool SetPropertySync(const char *name, const char *value);
	bool IsInitOnly(const char *name);
	std::vector<std::string> *ListPropertiesSync();

	bool ReopenFile();

	bool cancel;
	bool markEmmited;
	unsigned long long byteCount;
	int fd;
	std::ifstream *ifs;
	ResourceInputStream *stream;
	CondLock fileCond;	// for pause when source file is exhausted
	int markerResourceTypeId; // MarkerResource typeId
};

inline Module::Type Load::GetType() {
	return INPUT;
}

inline char *Load::GetPropertySync(const char *name) {
	return props->GetProperty(name);
}

inline bool Load::SetPropertySync(const char *name, const char *value) {
	return props->SetProperty(name, value);
}

inline std::vector<std::string> *Load::ListPropertiesSync() {
	return props->ListProperties();
}

#endif
