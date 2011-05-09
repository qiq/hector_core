/**
 * Class taking care of resources, it loads resource libraries, create resources, etc.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_REGISTRY_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_REGISTRY_H_

#include <config.h>

#include <deque>
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
	struct ResourceRegistryInfo {
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

	std::vector<std::string> *ParsePath(const char *path);
	std::vector<std::string> *ReadDir(const char *dir);
	ResourceRegistryInfo *LoadResourceLibrary(const char *typeString, int typeId);	// either typeString or typeId must by not-NULL/0
	ResourceRegistryInfo *GetInfo(int typeId);

	static PlainLock idLock;
	static int nextId;
	PlainLock infoLock;	// guargs name2id, id2info and all creating/changing of ResourceRegistryInfos
	std::tr1::unordered_map<std::string, int> name2id;
	std::tr1::unordered_map<int, ResourceRegistryInfo*> id2info;

	// files yet to be tested for resources
	std::deque<std::string> nativeFilenames;
	std::deque<std::string> perlFilenames;

	PerlInterpreters *perl;

	static log4cxx::LoggerPtr logger;
};

inline ResourceRegistry::ResourceRegistryInfo *ResourceRegistry::GetInfo(int typeId) {
	std::tr1::unordered_map<int, ResourceRegistryInfo*>::iterator iter = id2info.find(typeId);
	if (iter != id2info.end())
		return iter->second;
	return LoadResourceLibrary(NULL, typeId);
}

#endif
