/**
 * 
 */

#ifndef _LIB_PROCESSING_ENGINE_PERL_RESOURCE_H_
#define _LIB_PROCESSING_ENGINE_PERL_RESOURCE_H_

#include <config.h>

#include <string>
#include <vector>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <log4cxx/logger.h>
#include "EmbeddedPerl.h"
#include "IpAddr.h"
#include "PerlResourceInterpreter.h"
#include "PlainLock.h"
#include "Resource.h"

class PerlInfoMap {
public:
	PerlInfoMap() {};
	~PerlInfoMap();
	void Lock();
	void Unlock();
	ResourceInfo *GetResourceInfo(int typeId);
	void SetResourceInfo(ResourceInfo *info);
private:
	PlainLock infoLock;
	std::tr1::unordered_map<int, ResourceInfo*> infoMap;
};

class PerlResource : public Resource {
public:
	PerlResource(PerlResourceInterpreter *perl, const char *name);
	~PerlResource();
	bool Init();
	// create copy of a resource
	Resource *Clone();
	// Clear current resource (as would delete + new do)
	void Clear();
	// save and restore resource
	bool Serialize(ResourceOutputStream &output);
	bool Deserialize(ResourceInputStream &input);
	// used by queues in case there is limit on queue size
	int GetSize();
	// get info about this resource
	ResourceInfo *GetResourceInfo();
	// return string representation of the resource (e.g. for debugging purposes)
	std::string ToString(Object::LogLevel logLevel);

	std::string GetString(const std::string &name);
	int GetInt(const std::string &name);
	long GetLong(const std::string &name);
	IpAddr GetIpAddr(const std::string &name);
	std::string GetArrayString(const std::string &name, int index);
	int GetArrayInt(const std::string &name, int index);
	long GetArrayLong(const std::string &name, int index);
	IpAddr GetArrayIpAddr(const std::string &name, int index);
	std::string GetHashString(const std::string &name, const std::string &index);
	int GetHashInt(const std::string &name, const std::string &index);
	long GetHashLong(const std::string &name, const std::string &index);
	IpAddr GetHashIpAddr(const std::string &name, const std::string &index);

	void SetString(const std::string &name, const std::string &value);
	void SetInt(const std::string &name, int value);
	void SetLong(const std::string &name, long value);
	void SetIpAddr(const std::string &name, IpAddr &value);
	void SetArrayString(const std::string &name, int index, const std::string &value);
	void SetArrayInt(const std::string &name, int index, int value);
	void SetArrayLong(const std::string &name, int index, long value);
	void SetArrayIpAddr(const std::string &name, int index, IpAddr &value);
	void SetHashString(const std::string &name, const std::string &index, const std::string &value);
	void SetHashInt(const std::string &name, const std::string &index, int value);
	void SetHashLong(const std::string &name, const std::string &index, long value);
	void SetHashIpAddr(const std::string &name, const std::string &index, IpAddr &value);

	void Clear(const std::string&);
	int Count(const std::string&);
	void DeleteItem(const std::string&, const std::string&);
	std::vector<std::string> *Keys(const std::string&);
	std::vector<std::string> *StringValues(const std::string&);
	std::vector<int> *IntValues(const std::string&);
	std::vector<long> *LongValues(const std::string&);
	std::vector<IpAddr> *IpAddrValues(const std::string&);

	PerlResourceInterpreter *GetPerlResourceInterpreter();

protected:
	char *name;

	// interpreter the resource live within
	PerlResourceInterpreter *perl;
	// reference to the resource
	SV *ref;

	int typeId;
	static PerlInfoMap infoMap;

	SV *GetValue(const char *prefix, const char *name);
	SV *GetArrayValue(const char *name, int index);
	SV *GetHashValue(const char *name, const std::string &index);
	void SetValue(const char *name, SV *sv);
	void SetArrayValue(const char *name, int index, SV *sv);
	void SetHashValue(const char *name, const std::string &index, SV *sv);

	static log4cxx::LoggerPtr logger;
};

inline PerlResourceInterpreter *PerlResource::GetPerlResourceInterpreter() {
	return perl;
}

#endif
