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
	Module::Type getType();
	Resource *ProcessInput(bool sleep);
	bool SaveCheckpoint(const char *path);
	bool RestoreCheckpoint(const char *path);

	void Start();
	void Stop();

private:
	int items;		// ObjectLock
	int maxItems;		// ObjectLock
	char *filename;		// ObjectLock

	char *getItems(const char *name);
	char *getMaxItems(const char *name);
	void setMaxItems(const char *name, const char *value);
	char *getFilename(const char *name);
	void setFilename(const char *name, const char *value);

	ObjectValues<Load> *values;
	char *GetValue(const char *name);
	bool SetValue(const char *name, const char *value);
	bool IsInitOnly(const char *name);
	std::vector<std::string> *ListNames();

	bool cancel;
	unsigned long long byteCount;
	int fd;
	google::protobuf::io::FileInputStream *file;
	google::protobuf::io::CodedInputStream *stream;
	CondLock fileCond;	// for pause when source file is exhausted
};

inline Module::Type Load::getType() {
	return INPUT;
}

inline char *Load::GetValue(const char *name) {
	return values->GetValue(name);
}

inline bool Load::SetValue(const char *name, const char *value) {
	return values->SetValue(name, value);
}

inline bool Load::IsInitOnly(const char *name) {
	return values->IsInitOnly(name);
}

inline std::vector<std::string> *Load::ListNames() {
	return values->ListNames();
}

#endif
