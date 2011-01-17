/**
 * Class taking care of resources, it loads resource libraries, create resources, etc.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_REGISTRY_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_REGISTRY_H_

#include <config.h>

#include <limits>
#include <string>
#include <tr1/unordered_map>
#include <log4cxx/logger.h>
#include "PlainLock.h"

class ResourceInfo;
class Resource;

class ResourceRegistry {
public:
	ResourceRegistry() {};
	~ResourceRegistry();

	// methods common to all Resources
	int NameToId(const char *name);
	Resource *AcquireResource(int id);
	void ReleaseResource(Resource *resource);

protected:
	int LoadResourceLibrary(const char *name, int id);
	PlainLock translateLock;
	std::tr1::unordered_map<std::string, int> name2id;
	std::tr1::unordered_map<int, ResourceInfo*> id2info;

	static log4cxx::LoggerPtr logger;
};

#endif
