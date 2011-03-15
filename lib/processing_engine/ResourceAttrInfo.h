/**
 * Class representing a resource field, so that it can be used in a general way.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_ATTR_INFO_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_ATTR_INFO_H_

#include <config.h>

#include <string>
#include <vector>
#include "IpAddr.h"

class Resource;

class ResourceAttrInfo {
public:
	enum FieldType {
		UNKNOWN,
		STRING,
		INT,
		LONG,
		IP,		// IP address
		ARRAY_STRING,
		ARRAY_INT,
		ARRAY_LONG,
		ARRAY_IP,
		HASH_STRING,
		HASH_INT,
		HASH_LONG,
		HASH_IP,
	};

	ResourceAttrInfo(int typeId_): type(UNKNOWN), typeId(typeId_) {};
	virtual ~ResourceAttrInfo() {};
	FieldType GetType();
	void SetType(FieldType type);
	int GetTypeId();
	void SetTypeId(int id);

	virtual const std::string GetString(Resource*) = 0;
	virtual int GetInt(Resource*) = 0;
	virtual long GetLong(Resource*) = 0;
	virtual IpAddr GetIpAddr(Resource*) = 0;
	virtual const std::string GetArrayString(Resource*, int) = 0;
	virtual int GetArrayInt(Resource*, int) = 0;
	virtual long GetArrayLong(Resource*, int) = 0;
	virtual IpAddr GetArrayIpAddr(Resource*, int) = 0;
	virtual const std::string GetHashString(Resource*, const std::string&) = 0;
	virtual int GetHashInt(Resource*, const std::string&) = 0;
	virtual long GetHashLong(Resource*, const std::string&) = 0;
	virtual IpAddr GetHashIpAddr(Resource*, const std::string&) = 0;

	virtual void SetString(Resource*, const std::string&) = 0;
	virtual void SetInt(Resource*, int) = 0;
	virtual void SetLong(Resource*, long) = 0;
	virtual void SetIpAddr(Resource*, IpAddr&) = 0;
	virtual void SetArrayString(Resource*, int, const std::string&) = 0;
	virtual void SetArrayInt(Resource*, int, int) = 0;
	virtual void SetArrayLong(Resource*, int, long) = 0;
	virtual void SetArrayIpAddr(Resource*, int, IpAddr&) = 0;
	virtual void SetHashString(Resource*, const std::string&, const std::string&) = 0;
	virtual void SetHashInt(Resource*, const std::string&, int) = 0;
	virtual void SetHashLong(Resource*, const std::string&, long) = 0;
	virtual void SetHashIpAddr(Resource*, const std::string&, IpAddr&) = 0;

	virtual void Clear(Resource*) = 0;
	virtual void DeleteHashItem(Resource*, const std::string&) = 0;

	virtual int GetCount(Resource*) = 0;
	virtual std::vector<std::string> *GetKeys(Resource*) = 0;
	virtual std::vector<std::string> *GetValuesString(Resource*) = 0;
	virtual std::vector<int> *GetValuesInt(Resource*) = 0;
	virtual std::vector<long> *GetValuesLong(Resource*) = 0;
	virtual std::vector<IpAddr> *GetValuesIpAddr(Resource*) = 0;

protected:
	FieldType type;
	int typeId;		// resource typeId
	std::string name;
};

inline ResourceAttrInfo::FieldType ResourceAttrInfo::GetType() {
	return type;
}

inline void ResourceAttrInfo::SetType(ResourceAttrInfo::FieldType type) {
	this->type = type;
}

inline int ResourceAttrInfo::GetTypeId() {
	return typeId;
}

inline void ResourceAttrInfo::SetTypeId(int typeId) {
	this->typeId = typeId;
}

#endif
