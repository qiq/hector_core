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
	bool SaveCheckpointSync(const char *path);
	bool RestoreCheckpointSync(const char *path);

	void Start();
	void Stop();

private:
	int items;		// ObjectLock
	int maxItems;		// ObjectLock
	char *filename;		// ObjectLock
	int fd;			// ObjectLock
	bool cancel;		// ObjectLock
	google::protobuf::io::FileInputStream *stream;	// ObjectLock
	CondLock fileCond;	// for pause when source file is exhausted

	ObjectValues<Load> *values;

	char *getItems(const char *name);
	char *getMaxItems(const char *name);
	void setMaxItems(const char *name, const char *value);
	char *getFilename(const char *name);
	void setFilename(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	bool isInitOnly(const char *name);
	std::vector<std::string> *listNamesSync();

	bool ReadFromFile(void *data, int size, bool sleep);
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

inline bool Load::isInitOnly(const char *name) {
	return values->isInitOnly(name);
}

inline std::vector<std::string> *Load::listNamesSync() {
	return values->listNamesSync();
}

#endif
