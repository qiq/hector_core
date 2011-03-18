/**
 * Class taking care of resources, it loads resource libraries, create resources, etc.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_REGISTRY_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_REGISTRY_H_

#include <config.h>

#include <limits>
#include <string>
#include <utility>
#include <vector>
#include <tr1/unordered_map>
#include <log4cxx/logger.h>
#include "PlainLock.h"
#include "PerlInterpreters.h"

class Resource;
class Config;

class ResourceRegistry {
public:
	ResourceRegistry();
	~ResourceRegistry();

	// load resource libraries (lib, type)
	bool Load(const char *id, Config *config);

	// translate resource name to resource id
	int NameToId(const char *name);

	// obtain resource of given type
	Resource *AcquireResource(int typeId);

	// release resource
	void ReleaseResource(Resource *resource);

	// get attribute info of a resource
	ResourceAttrInfo *GetAttrInfo(int typeId, const char *name);

	// returns next free id
	static int NextResourceId();

protected:
	struct ResourceInfo {
		int typeId;
		std::string typeStr;
		Resource *(*create)();
		bool perl;
		bool python;
		char *library;
		std::vector<ResourceAttrInfo*> *attrInfo;
		std::tr1::unordered_map<std::string, ResourceAttrInfo*> attrMap;
		std::vector<Resource*> available;
	};

	static PlainLock idLock;
	static int nextId;
	PlainLock infoLock;
	std::tr1::unordered_map<std::string, int> name2id;
	std::tr1::unordered_map<int, ResourceInfo*> id2info;

	PerlInterpreters *perl;

	static log4cxx::LoggerPtr logger;
};

#endif
