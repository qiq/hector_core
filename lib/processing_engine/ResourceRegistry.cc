
#include <config.h>

#include <assert.h>
#include "Config.h"
#include "LibraryLoader.h"
#include "Resource.h"
#include "ResourceRegistry.h"
#include "PerlResource.h"
//#include "PythonResource.h"

using namespace std;

PlainLock ResourceRegistry::idLock;
int ResourceRegistry::nextId = 1;
log4cxx::LoggerPtr ResourceRegistry::logger(log4cxx::Logger::getLogger("ResourceRegistry"));

ResourceRegistry::ResourceRegistry() {
	perl = NULL;
}

ResourceRegistry::~ResourceRegistry() {
	for (tr1::unordered_map<int, ResourceInfo*>::iterator iter = id2info.begin(); iter != id2info.end(); ++iter) {
		vector<Resource*> *v = &iter->second->available;
		for (vector<Resource*>::iterator iter2 = v->begin(); iter2 != v->end(); ++iter2) {
			delete *iter2;
		}
		delete iter->second;
	}
	delete perl;
}

bool ResourceRegistry::Load(const char *id, Config *config) {
	char buffer[1024];
	vector<string> *v;
	snprintf(buffer, sizeof(buffer), "//Server/resources/Resource/@id");
	v = config->GetValues(buffer);
	if (v) {
		for (int i = 0; i < (int)v->size(); i++) {
			snprintf(buffer, sizeof(buffer), "//Resource[@id='%s']/@type", (*v)[i].c_str());
			char *type = config->GetFirstValue(buffer);
			snprintf(buffer, sizeof(buffer), "//Resource[@id='%s']/@lib", (*v)[i].c_str());
			char *lib = config->GetFirstValue(buffer);
			if (!lib) {
				LOG4CXX_ERROR(logger, "No library found for Resource " << (*v)[i]);
				return false;
			}

			ResourceInfo *info = new ResourceInfo();
			info->create = NULL;
			info->perl = false;
			info->python = false;
			Resource *r;
			if (!type || !strcmp(type, "native")) {
				info->create = (Resource*(*)())LibraryLoader::LoadLibrary(lib, "create");
				if (!info->create) {
					LOG4CXX_ERROR(logger, "Resource/lib not found: " << lib);
					return false;
				}
				r = (*info->create)();
			} else if (!strcmp(type, "perl")) {
				// Perl resource
				info->perl = true;
				if (!perl)
					perl = new PerlInterpreters();
				PerlResourceInterpreter *pi = perl->AcquirePerl();
				PerlResource *pr = new PerlResource(pi, lib);
				if (!pr->Init()) {
					LOG4CXX_ERROR(logger, "Cannot create resource of type: " << lib);
					return false;
				}
				r = pr;
			} else if (!strcmp(type, "python")) {
				// Python resource
				info->python = true;
//				PythonResorce *pr = new PythonResource(lib);
//				if (!pr->Init()) {
					LOG4CXX_ERROR(logger, "Cannot create resource of type: " << lib);
					return false;
//				}
			} else {
				LOG4CXX_ERROR(logger, "Unknown resource type (" << lib << ", " << type << ")");
				return false;
			}
			info->typeStr = r->GetTypeString();
			info->typeId = r->GetTypeId();
			info->attrInfo = r->GetAttrInfoList();
			for (vector<ResourceAttrInfo*>::iterator iter = info->attrInfo->begin(); iter != info->attrInfo->end(); ++iter)
				info->attrMap[info->typeStr] = *iter;
			name2id[info->typeStr] = info->typeId;
			id2info[info->typeId] = info;
			info->available.push_back(r);

			free(type);
			free(lib);
		}
		delete v;
	} else {
		LOG4CXX_ERROR(logger, "No resources config found");
		return false;
	}
	// reset id counter
	nextId = 1;
	return true;
}

/*bool ResourceRegistry::Load(vector<pair<string, string> > *libs) {
	for (vector<pair<string, string> >::iterator iter = libs->begin(); iter != libs->end(); ++iter) {
		ResourceInfo *info = new ResourceInfo();
		info->typeStr = iter->first;
		info->create = NULL;
		info->perl = false;
		info->python = false;
		const char *type = iter->second.c_str();
		Resource *r;
		if (!strcmp(type, "") || !strcmp(type, "native")) {
			info->create = (Resource*(*)())LibraryLoader::LoadLibrary(info->typeStr.c_str(), "create");
			if (!info->create) {
				LOG4CXX_ERROR(logger, "Resource/lib not found: " << info->typeStr);
				return false;
			}
			r = (*info->create)();
		} else if (!strcmp(type, "perl")) {
			// Perl resource
			info->perl = true;
			if (!perl)
				perl = new PerlInterpreters();
			PerlResourceInterpreter *pi = perl->AcquirePerl();
			PerlResource *pr = new PerlResource(pi, info->typeStr.c_str());
			if (!pr->Init()) {
				LOG4CXX_ERROR(logger, "Cannot create resource of type: " << info->typeStr);
				return false;
			}
			r = pr;
		} else if (!strcmp(type, "python")) {
			// Python resource
			info->python = true;
//			PythonResorce *pr = new PythonResource(info->typeStr);
//			if (!pr->Init()) {
//				LOG4CXX_ERROR(logger, "Cannot create resource of type: " << info->typeStr);
//				return false;
//			}
		} else {
			LOG4CXX_ERROR(logger, "Unknown resource type (" << info->typeStr << ", " << type << ")");
			return false;
		}
		info->typeId = r->GetTypeId();
		info->attrInfo = r->GetAttrInfoList();
		for (vector<ResourceAttrInfo*>::iterator iter = info->attrInfo->begin(); iter != info->attrInfo->end(); ++iter)
			info->attrMap[info->typeStr] = *iter;
		name2id[info->typeStr] = info->typeId;
		id2info[info->typeId] = info;
		info->available.push_back(r);
	}
	return true;
}*/

// Must be called with lock locked
// Either name or id must be non-NULL/0
/*int ResourceRegistry::LoadResourceLibrary(const char *name, int id) {
	// try to load library
	char s[1024];
	if (name)
		snprintf(s, sizeof(s), "%s.la", name);
	else
		snprintf(s, sizeof(s), "%d.la", id);
	Resource *(*create)() = (Resource*(*)())LibraryLoader::LoadLibrary(s, "create");
	if (!create) {
		if (name) {
			LOG4CXX_ERROR(logger, "Invalid Resource name: " << name);
		} else {
			LOG4CXX_ERROR(logger, "Invalid Resource Id: " << id);
		}
		return false;
	}
	// get id and save it into name2id
	Resource *r = (*create)();
	id = r->getTypeId();
	name = r->getTypeStr();
	name2id[name] = id;
	ResourceInfo *info = new ResourceInfo();
	// save constructor reference and store first available resource
	info->create = create;
	info->available.push_back(r);
	id2info[id] = info;
	return true;
}*/

int ResourceRegistry::NameToId(const char *name) {
	tr1::unordered_map<string, int>::iterator iter = name2id.find(name);
	if (iter != name2id.end())
		return iter->second;

//	LOG4CXX_ERROR(logger, "Invalid Resource name: " << name);
	return -1;
}

Resource *ResourceRegistry::AcquireResource(int typeId) {
	if (typeId < 0)
		return NULL;
	tr1::unordered_map<int, ResourceInfo*>::iterator iter = id2info.find(typeId);
	if (iter == id2info.end()) {
//		LOG4CXX_ERROR(logger, "Unknown resource type: " << typeId);
		return NULL;
	}
	Resource *result;
	infoLock.Lock();
	if (iter->second->available.size() > 0) {
		result = iter->second->available.back();
		iter->second->available.pop_back();
		result->SetId(NextResourceId());
		infoLock.Unlock();
	} else {
		infoLock.Unlock();
 		result = iter->second->create();
	}
	return result;
}

#define AVAILABLE_SIZE_MAX 10000
void ResourceRegistry::ReleaseResource(Resource *resource) {
	if (!resource)
		return;
	tr1::unordered_map<int, ResourceInfo*>::iterator iter = id2info.find(resource->GetTypeId());
	if (iter == id2info.end()) {
		LOG4CXX_ERROR(logger, "Unknown resource type: " << resource->GetTypeId());
		return;
	}
	infoLock.Lock();
	if (iter->second->available.size() < AVAILABLE_SIZE_MAX) {
		resource->Clear();
		iter->second->available.push_back(resource);
		infoLock.Unlock();
	} else {
		infoLock.Unlock();
		delete resource;
	}
}

ResourceAttrInfo *ResourceRegistry::GetAttrInfo(int typeId, const char *name) {
	tr1::unordered_map<int, ResourceInfo*>::iterator iter = id2info.find(typeId);
	if (iter == id2info.end())
		return NULL;
	ResourceInfo *info = iter->second;
	tr1::unordered_map<string, ResourceAttrInfo*>::iterator iter2 = info->attrMap.find(info->typeStr.c_str());
	if (iter2 == info->attrMap.end())
		return NULL;
	return iter2->second;
}

int ResourceRegistry::NextResourceId() {
	idLock.Lock();
	int next = nextId++;
	idLock.Unlock();
	return next;
}

