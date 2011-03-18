
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
			if (iter->second->perl) {
				PerlResource *pr = static_cast<PerlResource*>(*iter2);
				perl->ReleasePerl(pr->GetPerlResourceInterpreter());
				delete pr;
			} else if (iter->second->python) {
				// TODO
			} else {
				delete *iter2;
			}
		}
		for (vector<ResourceAttrInfo*>::iterator iter2 = iter->second->attrInfo->begin(); iter2 != iter->second->attrInfo->end(); ++iter2)
			delete *iter2;
		delete iter->second->attrInfo;
		free(iter->second->library);
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
			info->library = NULL;
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
				info->library = strdup(lib);
				if (!perl)
					perl = new PerlInterpreters();
				PerlResourceInterpreter *pi = perl->AcquirePerl();
				if (!pi) {
					LOG4CXX_ERROR(logger, "Cannot create perl Instance");
					return false;
				}
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
				info->attrMap[(*iter)->GetName()] = *iter;
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

int ResourceRegistry::NameToId(const char *name) {
	tr1::unordered_map<string, int>::iterator iter = name2id.find(name);
	if (iter != name2id.end())
		return iter->second;

	return -1;
}

Resource *ResourceRegistry::AcquireResource(int typeId) {
	if (typeId < 0)
		return NULL;
	tr1::unordered_map<int, ResourceInfo*>::iterator iter = id2info.find(typeId);
	if (iter == id2info.end())
		return NULL;
	Resource *result;
	infoLock.Lock();
	ResourceInfo *info = iter->second;
	// previously created resource?
	if (info->available.size() > 0) {
		result = info->available.back();
		info->available.pop_back();
		infoLock.Unlock();
		result->SetId(NextResourceId());
	} else {
		infoLock.Unlock();
		// previously created resource not available
		if (info->perl) {
			PerlResourceInterpreter *pi = perl->AcquirePerl();
			if (!pi) {
				LOG4CXX_ERROR(logger, "Cannot create perl instance");
				return false;
			}
			PerlResource *pr = new PerlResource(pi, info->library);
			if (!pr->Init()) {
				LOG4CXX_ERROR(logger, "Cannot create resource of type: " << info->library);
				return false;
			}
			result = pr;
		} else if (info->python) {
			// TODO
		} else {
 			result = iter->second->create();
		}
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
	ResourceInfo *info = iter->second;
	if (info->available.size() < AVAILABLE_SIZE_MAX) {
		resource->Clear();
		info->available.push_back(resource);
		infoLock.Unlock();
	} else {
		infoLock.Unlock();
		if (info->perl) {
			PerlResource *pr = static_cast<PerlResource*>(resource);
			perl->ReleasePerl(pr->GetPerlResourceInterpreter());
			delete pr;
		} else if (info->python) {
			// TODO
		} else {
			delete resource;
		}
	}
}

ResourceAttrInfo *ResourceRegistry::GetAttrInfo(int typeId, const char *name) {
	tr1::unordered_map<int, ResourceInfo*>::iterator iter = id2info.find(typeId);
	if (iter == id2info.end())
		return NULL;
	ResourceInfo *info = iter->second;
	tr1::unordered_map<string, ResourceAttrInfo*>::iterator iter2 = info->attrMap.find(name);
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

