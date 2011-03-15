/**
Load.la, input, native
Load Resources from the file.

Dependencies: protobuf

Parameters:
items		r/o	Total items processed
maxItems	r/w	Number of items to load
filename	r/w	File to load. Change it to process another file
*/

#ifndef _LIB_PROCESSING_ENGINE_MODULES_LOAD_H_
#define _LIB_PROCESSING_ENGINE_MODULES_LOAD_H_

#include <config.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "CondLock.h"
#include "Module.h"
#include "ObjectValues.h"

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
	int items;		// ObjectLock
	int maxItems;		// ObjectLock
	char *filename;		// ObjectLock

	char *GetItems(const char *name);
	char *GetMaxItems(const char *name);
	void SetMaxItems(const char *name, const char *value);
	char *GetFilename(const char *name);
	void SetFilename(const char *name, const char *value);

	ObjectValues<Load> *values;
	char *GetValueSync(const char *name);
	bool SetValueSync(const char *name, const char *value);
	bool IsInitOnly(const char *name);
	std::vector<std::string> *ListNamesSync();

	bool cancel;
	unsigned long long byteCount;
	int fd;
	google::protobuf::io::FileInputStream *file;
	google::protobuf::io::CodedInputStream *stream;
	CondLock fileCond;	// for pause when source file is exhausted
};

inline Module::Type Load::GetType() {
	return INPUT;
}

inline char *Load::GetValueSync(const char *name) {
	return values->GetValue(name);
}

inline bool Load::SetValueSync(const char *name, const char *value) {
	return values->SetValue(name, value);
}

inline std::vector<std::string> *Load::ListNamesSync() {
	return values->ListNames();
}

#endif
