/**
Save.la, output, native
Save Resources to the file.

Dependencies: protobuf

Parameters:
items		r/o		Total items processed
filename	initOnly	File to save resources to.
overwrite	initOnly	Should we overwrite output file?
saveResourceType	r/w		Save type info (necessary if we are writing different resource types to one file)
saveResourceIdStatus	r/w		Save Id & Status attributes to the file (usualy not desirable)
*/

#ifndef _LIB_PROCESSING_ENGINE_MODULES_SAVE_H_
#define _LIB_PROCESSING_ENGINE_MODULES_SAVE_H_

#include <config.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "Module.h"
#include "ObjectProperties.h"
#include "Resource.h"
#include "ResourceOutputStream.h"

class Save : public Module {
public:
	Save(ObjectRegistry *objects, const char *id, int threadIndex);
	~Save();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type GetType();
	Resource *ProcessOutputSync(Resource *resource);

private:
	int items;
	char *filename;
	bool overwrite;
	bool saveResourceType;
	bool saveResourceIdStatus;

	char *GetItems(const char *name);
	char *GetFilename(const char *name);
	void SetFilename(const char *name, const char *value);
	char *GetOverwrite(const char *name);
	void SetOverwrite(const char *name, const char *value);
	char *GetSaveResourceType(const char *name);
	void SetSaveResourceType(const char *name, const char *value);
	char *GetSaveResourceIdStatus(const char *name);
	void SetSaveResourceIdStatus(const char *name, const char *value);

	ObjectProperties<Save> *props;
	char *GetPropertySync(const char *name);
	bool SetPropertySync(const char *name, const char *value);
	std::vector<std::string> *ListPropertiesSync();

	int fd;
	ResourceOutputStream *stream;
};

inline Module::Type Save::GetType() {
	return OUTPUT;
}

inline char *Save::GetPropertySync(const char *name) {
	return props->GetProperty(name);
}

inline bool Save::SetPropertySync(const char *name, const char *value) {
	return props->SetProperty(name, value);
}

inline std::vector<std::string> *Save::ListPropertiesSync() {
	return props->ListProperties();
}

#endif
