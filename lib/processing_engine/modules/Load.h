/**
 * Test module, does nothing.
 */

#ifndef _LIB_PROCESSING_ENGINE_MODULES_LOAD_H_
#define _LIB_PROCESSING_ENGINE_MODULES_LOAD_H_

#include <config.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "Module.h"
#include "ObjectValues.h"

class Load : public Module {
public:
	Load(ObjectRegistry *objects, const char *id, int threadIndex);
	~Load();
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	Module::Type getType();
	Resource *ProcessInput(bool sleep);

private:
	int items;		// guarded by ObjectLock
	int maxItems;		// guarded by ObjectLock
	char *filename;		// guarded by ObjectLock
	int fd;			// private to ProcessInput()
	google::protobuf::io::FileInputStream *stream;	// private to ProcessInput()

	ObjectValues<Load> *values;

	char *getItems(const char *name);
	char *getMaxItems(const char *name);
	void setMaxItems(const char *name, const char *value);
	char *getFilename(const char *name);
	void setFilename(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	std::vector<std::string> *listNamesSync();

	bool ReadFromFile(void *data, int size);
};

inline Module::Type Load::getType() {
	return INPUT;
}

inline char *Load::getValueSync(const char *name) {
	return values->getValueSync(name);
}

inline bool Load::setValueSync(const char *name, const char *value) {
	return values->setValueSync(name, value);
}

inline std::vector<std::string> *Load::listNamesSync() {
	return values->listNamesSync();
}

#endif
