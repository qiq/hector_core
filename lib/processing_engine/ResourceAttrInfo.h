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
	typedef enum {
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
	} FieldType;

	ResourceAttrInfo();
	virtual ~ResourceAttrInfo() {};
	FieldType getType();
	std::string &getName();

	virtual const std::string &getString(Resource*) = 0;
	virtual int getInt(Resource*) = 0;
	virtual long getLong(Resource*) = 0;
	virtual IpAddr &getIpAddr(Resource*) = 0;
	virtual const std::string &getArrayString(Resource*, int) = 0;
	virtual int getArrayInt(Resource*, int) = 0;
	virtual long getArrayLong(Resource*, int) = 0;
	virtual IpAddr &getArrayIpAddr(Resource*, int) = 0;
	virtual const std::string &getHashString(Resource*, const std::string&) = 0;
	virtual int getHashInt(Resource*, const std::string&) = 0;
	virtual long getHashLong(Resource*, const std::string&) = 0;
	virtual IpAddr &getHashIpAddr(Resource*, const std::string&) = 0;

	virtual void setString(Resource*, const std::string&) = 0;
	virtual void setInt(Resource*, int) = 0;
	virtual void setLong(Resource*, long) = 0;
	virtual void setIpAddr(Resource*, IpAddr&) = 0;
	virtual void setArrayString(Resource*, int, const std::string&) = 0;
	virtual void setArrayInt(Resource*, int, int) = 0;
	virtual void setArrayLong(Resource*, int, long) = 0;
	virtual void setArrayIpAddr(Resource*, int, IpAddr&) = 0;
	virtual void setHashString(Resource*, const std::string&, const std::string&) = 0;
	virtual void setHashInt(Resource*, const std::string&, int) = 0;
	virtual void setHashLong(Resource*, const std::string&, long) = 0;
	virtual void setHashIpAddr(Resource*, const std::string&, IpAddr&) = 0;

	virtual void clear(Resource*) = 0;
	virtual void deleteHashItem(Resource*, const std::string&) = 0;

	virtual int getCount(Resource*) = 0;
	virtual std::vector<std::string> *getKeys(Resource*) = 0;
	virtual std::vector<std::string> *getValuesString(Resource*) = 0;
	virtual std::vector<int> *getValuesInt(Resource*) = 0;
	virtual std::vector<long> *getValuesLong(Resource*) = 0;
	virtual std::vector<IpAddr> *getValuesIpAddr(Resource*) = 0;

protected:
	FieldType type;
	std::string name;
};

inline ResourceAttrInfo::ResourceAttrInfo() {
	type = UNKNOWN;
}

inline ResourceAttrInfo::FieldType ResourceAttrInfo::getType() {
	return type;
}

inline std::string &ResourceAttrInfo::getName() {
	return name;
}

#endif
