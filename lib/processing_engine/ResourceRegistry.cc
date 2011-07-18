
#include <config.h>

#include <assert.h>
#include <stdlib.h>
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

vector<string> *ResourceRegistry::ParsePath(const char *path) {
	vector<string> *result = new vector<string>();
	char *copy = strdup(path);
	char *s = copy;
	while (*s) {
		char *path = s;
		while (*s && *s != ':')
			s++;
		if (*s)
			*s++ = '\0';
		result->push_back(path);
	}
	free(copy);
	return result;
}

vector<string> *ResourceRegistry::ReadDir(const char *dir) {
	vector<string> *result = new vector<string>();
	DIR *dp;
	struct dirent *de;
	if (!(dp = opendir(dir)))
		return result;
	while ((de = readdir(dp))) {
		result->push_back(string(de->d_name));
	}
	closedir(dp);
	return result;
}

ResourceRegistry::ResourceRegistry() {
	// native resource candidates
	char *s = getenv("LD_LIBRARY_PATH");
	if (s) {
		vector<string> *paths = ParsePath(s);
		for (vector<string>::iterator iter = paths->begin(); iter != paths->end(); ++iter) {
			vector<string> *files = ReadDir(iter->c_str());
			for (vector<string>::iterator iter2 = files->begin(); iter2 != files->end(); ++iter2) {
				if (iter2->length() < 4 || memcmp(iter2->data()+iter2->length()-3, ".la", 3))
					continue;	// invalid file name
				nativeFilenames.push_back(*iter2);
			}
			delete files;
		}
		delete paths;
	}
	// Perl resource candidates
	s = getenv("PERL5LIB");
	if (s) {
		vector<string> *paths = ParsePath(s);
		for (vector<string>::iterator iter = paths->begin(); iter != paths->end(); ++iter) {
			vector<string> *files = ReadDir(iter->c_str());
			for (vector<string>::iterator iter2 = files->begin(); iter2 != files->end(); ++iter2) {
				if (iter2->length() < 4 || memcmp(iter2->data()+iter2->length()-3, ".pm", 3))
					continue;	// invalid file name
				perlFilenames.push_back(*iter2);
			}
			delete files;
		}
		delete paths;
	}

	perl = NULL;
	// reset id counter
	nextId = 1;
}

ResourceRegistry::~ResourceRegistry() {
	for (tr1::unordered_map<int, ResourceRegistryInfo*>::iterator iter = id2info.begin(); iter != id2info.end(); ++iter) {
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
		free(iter->second->library);
		delete iter->second;
	}
	delete perl;
}

// infoLock must be held
ResourceRegistry::ResourceRegistryInfo *ResourceRegistry::LoadResourceLibrary(const char *typeString, int typeId) {
	// either typeString or typeId must by not-NULL/0
	assert(typeString || typeId != 0);

	// try to find native library for given resource
	while (nativeFilenames.size() > 0) {
		string filename = nativeFilenames.front();
		nativeFilenames.pop_front();
		// try to load library
		Resource *(*create)() = (Resource*(*)())LibraryLoader::LoadLibrary(filename.c_str(), "hector_resource_create", false);
		if (!create)
			continue;
		// create mapping
		Resource *r = (*create)();
		if (!r)
			continue;
		ResourceInfo *ri = r->GetResourceInfo();
		if (!ri)
			continue;
		tr1::unordered_map<int, ResourceRegistryInfo*>::iterator iter = id2info.find(ri->GetTypeId());
		ResourceRegistryInfo *info;
		if (iter == id2info.end()) {
			info = new ResourceRegistryInfo();
			info->typeId = ri->GetTypeId();
			info->typeStr = ri->GetTypeString();
			info->create = create;
			info->perl = false;
			info->python = false;
			info->library = NULL;
			vector<ResourceAttrInfo*> *attrInfo = ri->GetAttrInfoList();
			for (vector<ResourceAttrInfo*>::iterator iter = attrInfo->begin(); iter != attrInfo->end(); ++iter)
				info->attrMap[(*iter)->GetName()] = *iter;
			name2id[info->typeStr] = info->typeId;
			id2info[info->typeId] = info;
		} else {
			info = iter->second;
		}
		info->available.push_back(r);

		if ((typeId && info->typeId == typeId) || (typeString && !strcmp(typeString, info->typeStr.c_str())))
			return info;
	}

	// try to find Perl resource using PERL5LIB
	while (perlFilenames.size() > 0) {
		string filename = perlFilenames.front();
		perlFilenames.pop_front();
		// try to load library
		if (!perl)
			perl = new PerlInterpreters();
		PerlResourceInterpreter *pi = perl->AcquirePerl();
		if (!pi) {
			LOG4CXX_ERROR(logger, "Cannot create Perl Instance");
			perlFilenames.clear();
			break;
		}
		PerlResource *pr = new PerlResource(pi, filename.c_str());
		if (!pr->Init(false)) {
			delete pr;
			continue;
		}
		ResourceInfo *ri = pr->GetResourceInfo();
		if (!ri)
			continue;
		tr1::unordered_map<int, ResourceRegistryInfo*>::iterator iter = id2info.find(ri->GetTypeId());
		ResourceRegistryInfo *info;
		if (iter == id2info.end()) {
			info = new ResourceRegistryInfo();
			info->typeId = ri->GetTypeId();
			info->typeStr = ri->GetTypeString();
			info->create = NULL;
			info->perl = true;
			info->python = false;
			info->library = strdup(filename.c_str());
			vector<ResourceAttrInfo*> *attrInfo = ri->GetAttrInfoList();
			for (vector<ResourceAttrInfo*>::iterator iter = attrInfo->begin(); iter != attrInfo->end(); ++iter)
				info->attrMap[(*iter)->GetName()] = *iter;
			name2id[info->typeStr] = info->typeId;
			id2info[info->typeId] = info;
		} else {
			info = iter->second;
		}
		info->available.push_back(pr);

		if ((typeId && info->typeId == typeId) || (typeString && !strcmp(typeString, info->typeStr.c_str())))
			return info;
	}

	// resource library was not found
	return NULL;
}

int ResourceRegistry::NameToId(const char *name) {
	infoLock.Lock();
	tr1::unordered_map<string, int>::iterator iter = name2id.find(name);
	if (iter != name2id.end()) {
		int result = iter->second;
		infoLock.Unlock();
		return result;
	}

	ResourceRegistryInfo *info = LoadResourceLibrary(name, 0);
	if (info) {
		int result = info->typeId;
		infoLock.Unlock();
		return result;
	}
	infoLock.Unlock();

	return -1;
}

Resource *ResourceRegistry::AcquireResource(int typeId) {
	if (typeId < 0)
		return NULL;

	infoLock.Lock();
	ResourceRegistryInfo *info = GetInfo(typeId);
	if (!info) {
		infoLock.Unlock();
		return NULL;
	}
	Resource *result;
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
 			result = info->create();
		}
	}
	return result;
}

#define AVAILABLE_SIZE_MAX 10000
void ResourceRegistry::ReleaseResource(Resource *resource) {
	if (!resource)
		return;
	infoLock.Lock();
	ResourceRegistryInfo *info = GetInfo(resource->GetTypeId());
	if (!info) {
		LOG4CXX_ERROR(logger, "Unknown resource type: " << resource->GetTypeId());
		infoLock.Unlock();
		return;
	}
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
	infoLock.Lock();
	ResourceRegistryInfo *info = GetInfo(typeId);
	infoLock.Unlock();
	if (!info)
		return NULL;
	tr1::unordered_map<string, ResourceAttrInfo*>::iterator iter = info->attrMap.find(name);
	if (iter == info->attrMap.end())
		return NULL;
	return iter->second;
}

int ResourceRegistry::NextResourceId() {
	idLock.Lock();
	int next = nextId++;
	idLock.Unlock();
	return next;
}

