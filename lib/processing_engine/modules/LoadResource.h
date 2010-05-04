/**
 * Test module, does nothing.
 */

#ifndef _LOAD_RESOURCE_H_
#define _LOAD_RESOURCE_H_

#include <config.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "Module.h"
#include "ObjectValues.h"

class LoadResource : public Module {
public:
	LoadResource(ObjectRegistry *objects, const char *id, int threadIndex);
	~LoadResource();
	bool Init(vector<pair<string, string> > *params);
	Module::Type getType();
	Resource *Process(Resource *resource);

private:
	int items;		// guarded by ObjectLock
	int maxItems;		// guarded by ObjectLock
	char *filename;		// guarded by ObjectLock
	int fd;			// private to Process()
	google::protobuf::io::FileInputStream *stream;	// private to Process()

	ObjectValues<LoadResource> *values;

	char *getItems(const char *name);
	char *getMaxItems(const char *name);
	void setMaxItems(const char *name, const char *value);
	char *getFilename(const char *name);
	void setFilename(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	vector<string> *listNamesSync();

	bool ReadFromFile(void *data, int size);
};

inline Module::Type LoadResource::getType() {
	return INPUT;
}

inline char *LoadResource::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool LoadResource::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline vector<string> *LoadResource::listNamesSync() {
	return values->listNamesSync();
}

#endif
