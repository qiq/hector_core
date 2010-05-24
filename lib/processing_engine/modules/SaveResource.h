/**
 * Test module, does nothing.
 */

#ifndef _SAVE_RESOURCE_H_
#define _SAVE_RESOURCE_H_

#include <config.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "Module.h"
#include "ObjectValues.h"

class SaveResource : public Module {
public:
	SaveResource(ObjectRegistry *objects, const char *id, int threadIndex);
	~SaveResource();
	bool Init(vector<pair<string, string> > *params);
	Module::Type getType();
	Resource *Process(Resource *resource);

private:
	int items;		// guarded by ObjectLock
	char *filename;		// guarded by ObjectLock
	int fd;			// private to Process()
	google::protobuf::io::FileOutputStream *stream;	// private to Process()

	ObjectValues<SaveResource> *values;

	char *getItems(const char *name);
	char *getFilename(const char *name);
	void setFilename(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	vector<string> *listNamesSync();

	bool WriteToFile(const void *data, int size);
};

inline Module::Type SaveResource::getType() {
	return OUTPUT;
}

inline char *SaveResource::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool SaveResource::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline vector<string> *SaveResource::listNamesSync() {
	return values->listNamesSync();
}

#endif
