// List of all resource types

#ifndef _LIB_PROCESSING_ENGINE_RESOURCES_H_
#define _LIB_PROCESSING_ENGINE_RESOURCES_H_

#include <config.h>

#include <queue>
#include <string>
#include <tr1/unordered_map>
#include <log4cxx/logger.h>
#include "PlainLock.h"
#include "Resource.h"

class Resources {
public:
	Resources() {};
	~Resources() {};

	static Resource *CreateResource(int id);
	static int NameToId(const char *name);
private:
	static int LoadResourceLibrary(const char *name, int id);

	static PlainLock lock;
	static std::tr1::unordered_map<std::string, int> name2id;
	static std::tr1::unordered_map<int, Resource *(*)()> id2create;
	static log4cxx::LoggerPtr logger;
};

#endif
