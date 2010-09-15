/**
 * Test module, does nothing.
 */

#ifndef _SAVE_H_
#define _SAVE_H_

#include <config.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "Module.h"
#include "ObjectValues.h"

class Save : public Module {
public:
	Save(ObjectRegistry *objects, const char *id, int threadIndex);
	~Save();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type getType();
	Resource *Process(Resource *resource);

private:
	int items;		// guarded by ObjectLock
	char *filename;		// guarded by ObjectLock
	int fd;			// private to Process()
	google::protobuf::io::FileOutputStream *stream;	// private to Process()

	ObjectValues<Save> *values;

	char *getItems(const char *name);
	char *getFilename(const char *name);
	void setFilename(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	std::vector<std::string> *listNamesSync();

	bool WriteToFile(const void *data, int size);
};

inline Module::Type Save::getType() {
	return OUTPUT;
}

inline char *Save::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool Save::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline std::vector<std::string> *Save::listNamesSync() {
	return values->listNamesSync();
}

#endif
