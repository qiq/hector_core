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

class PerlResource : public Resource {
public:
	PerlResource(PerlResourceInterpreter *perl, const char *name);
	~PerlResource();
	bool Init(bool reportErrors = true);
	// create copy of a resource
	Resource *Clone();
	// Clear current resource (as would delete + new do)
	void Clear();
	// save and restore resource
	bool Serialize(ResourceOutputStream &output);
	bool Deserialize(ResourceInputStream &input, bool headerOnly);
	bool Skip(ResourceInputStream &input);
	// used by queues in case there is limit on queue size
	int GetSize();
	// get info about this resource
	ResourceInfo *GetResourceInfo();
	// return string representation of the resource (e.g. for debugging purposes)
	std::string ToString(Object::LogLevel logLevel);

	int32_t GetInt32(const std::string &name);
	uint32_t GetUInt32(const std::string &name);
	int64_t GetInt64(const std::string &name);
	uint64_t GetUInt64(const std::string &name);
	double GetDouble(const std::string &name);
	std::string GetString(const std::string &name);
	IpAddr GetIpAddr(const std::string &name);
	int32_t GetArrayInt32(const std::string &name, int index);
	uint32_t GetArrayUInt32(const std::string &name, int index);
	int64_t GetArrayInt64(const std::string &name, int index);
	uint64_t GetArrayUInt64(const std::string &name, int index);
	double GetArrayDouble(const std::string &name, int index);
	std::string GetArrayString(const std::string &name, int index);
	IpAddr GetArrayIpAddr(const std::string &name, int index);
	int32_t GetHashInt32(const std::string &name, const std::string &index);
	uint32_t GetHashUInt32(const std::string &name, const std::string &index);
	int64_t GetHashInt64(const std::string &name, const std::string &index);
	uint64_t GetHashUInt64(const std::string &name, const std::string &index);
	double GetHashDouble(const std::string &name, const std::string &index);
	std::string GetHashString(const std::string &name, const std::string &index);
	IpAddr GetHashIpAddr(const std::string &name, const std::string &index);

	void SetInt32(const std::string &name, int32_t value);
	void SetUInt32(const std::string &name, uint32_t value);
	void SetInt64(const std::string &name, int64_t value);
	void SetUInt64(const std::string &name, uint64_t value);
	void SetDouble(const std::string &name, double value);
	void SetString(const std::string &name, const std::string &value);
	void SetIpAddr(const std::string &name, IpAddr &value);
	void SetArrayInt32(const std::string &name, int index, int32_t value);
	void SetArrayUInt32(const std::string &name, int index, uint32_t value);
	void SetArrayInt64(const std::string &name, int index, int64_t value);
	void SetArrayUInt64(const std::string &name, int index, uint64_t value);
	void SetArrayDouble(const std::string &name, int index, double value);
	void SetArrayString(const std::string &name, int index, const std::string &value);
	void SetArrayIpAddr(const std::string &name, int index, IpAddr &value);
	void SetHashInt32(const std::string &name, const std::string &index, int32_t value);
	void SetHashUInt32(const std::string &name, const std::string &index, uint32_t value);
	void SetHashInt64(const std::string &name, const std::string &index, int64_t value);
	void SetHashUInt64(const std::string &name, const std::string &index, uint64_t value);
	void SetHashDouble(const std::string &name, const std::string &index, double value);
	void SetHashString(const std::string &name, const std::string &index, const std::string &value);
	void SetHashIpAddr(const std::string &name, const std::string &index, IpAddr &value);

	void Clear(const std::string&);
	int Count(const std::string&);
	void DeleteItem(const std::string&, const std::string&);
	std::vector<std::string> *Keys(const std::string&);
	std::vector<int32_t> *Int32Values(const std::string&);
	std::vector<uint32_t> *UInt32Values(const std::string&);
	std::vector<int64_t> *Int64Values(const std::string&);
	std::vector<uint64_t> *UInt64Values(const std::string&);
	std::vector<double> *DoubleValues(const std::string&);
	std::vector<std::string> *StringValues(const std::string&);
	std::vector<IpAddr> *IpAddrValues(const std::string&);

	PerlResourceInterpreter *GetPerlResourceInterpreter();

protected:
	char *name;

	// interpreter the resource live within
	PerlResourceInterpreter *perl;
	// reference to the resource
	SV *ref;

	PlainLock riLock;
	ResourceInfo *resourceInfo;

	SV *GetValue(const char *prefix, const char *name);
	SV *GetArrayValue(const char *name, int index);
	SV *GetHashValue(const char *name, const std::string &index);
	void SetValue(const char *name, SV *sv);
	void SetArrayValue(const char *name, int index, SV *sv);
	void SetHashValue(const char *name, const std::string &index, SV *sv);

	long GetLong(const std::string &name);
	long GetArrayLong(const std::string &name, int index);
	long GetHashLong(const std::string &name, const std::string &index);
	void SetLong(const std::string &name, long value);
	void SetArrayLong(const std::string &name, int index, long value);
	void SetHashLong(const std::string &name, const std::string &index, long value);

	static log4cxx::LoggerPtr logger;
};

inline PerlResourceInterpreter *PerlResource::GetPerlResourceInterpreter() {
	return perl;
}

#endif
