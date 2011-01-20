/**
Save.la, output, native
Save Resources to the file.

Dependencies: protobuf

Parameters:
items		r/o		Total items processed
filename	initOnly	File to save resources to.
*/

#ifndef _LIB_PROCESSING_ENGINE_MODULES_SAVE_H_
#define _LIB_PROCESSING_ENGINE_MODULES_SAVE_H_

#include <config.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "Module.h"
#include "ObjectValues.h"

class Save : public Module {
public:
	Save(ObjectRegistry *objects, const char *id, int threadIndex);
	~Save();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type getType();
	Resource *ProcessOutputSync(Resource *resource);

private:
	int items;		// ObjectLock
	char *filename;		// initOnly

	char *getItems(const char *name);
	char *getFilename(const char *name);
	void setFilename(const char *name, const char *value);

	ObjectValues<Save> *values;
	char *GetValueSync(const char *name);
	bool SetValueSync(const char *name, const char *value);
	std::vector<std::string> *ListNamesSync();

	int fd;
	google::protobuf::io::ZeroCopyOutputStream *file;
	google::protobuf::io::CodedOutputStream *stream;
};

inline Module::Type Save::getType() {
	return OUTPUT;
}

inline char *Save::GetValueSync(const char *name) {
	return values->GetValue(name);
}

inline bool Save::SetValueSync(const char *name, const char *value) {
	return values->SetValue(name, value);
}

inline std::vector<std::string> *Save::ListNamesSync() {
	return values->ListNames();
}

#endif
