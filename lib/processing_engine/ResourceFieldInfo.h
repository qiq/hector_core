/**
 * Class representing a resource field, so that it can be used in a general way.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_FIELD_INFO_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_FIELD_INFO_H_

#include <config.h>

#include <assert.h>
#include <string>
#include "Resource.h"
#include "IpAddr.h"

class ResourceFieldInfo {
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

	ResourceFieldInfo() : type(UNKNOWN) {};
	virtual ~ResourceFieldInfo() {};
	FieldType getType();

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
};

template<class T>
class ResourceFieldInfoT : public ResourceFieldInfo {
public:

	ResourceFieldInfoT(const std::string &name);
	~ResourceFieldInfoT() {};

	const std::string &getString(Resource*);
	int getInt(Resource*);
	long getLong(Resource*);
	IpAddr &getIpAddr(Resource*);
	const std::string &getArrayString(Resource*, int);
	int getArrayInt(Resource*, int);
	long getArrayLong(Resource*, int);
	IpAddr &getArrayIpAddr(Resource*, int);
	const std::string &getHashString(Resource*, const std::string&);
	int getHashInt(Resource*, const std::string&);
	long getHashLong(Resource*, const std::string&);
	IpAddr &getHashIpAddr(Resource*, const std::string&);

	void setString(Resource*, const std::string&);
	void setInt(Resource*, int);
	void setLong(Resource*, long);
	void setIpAddr(Resource*, IpAddr&);
	void setArrayString(Resource*, int, const std::string&);
	void setArrayInt(Resource*, int, int);
	void setArrayLong(Resource*, int, long);
	void setArrayIpAddr(Resource*, int, IpAddr&);
	void setHashString(Resource*, const std::string&, const std::string&);
	void setHashInt(Resource*, const std::string&, int);
	void setHashLong(Resource*, const std::string&, long);
	void setHashIpAddr(Resource*, const std::string&, IpAddr&);

	void clear(Resource*);
	void deleteHashItem(Resource*, const std::string&);

	int getCount(Resource*);
	std::vector<std::string> *getKeys(Resource*);
	std::vector<std::string> *getValuesString(Resource*);
	std::vector<int> *getValuesInt(Resource*);
	std::vector<long> *getValuesLong(Resource*);
	std::vector<IpAddr> *getValuesIpAddr(Resource*);

protected:
	union {
		const std::string &(T::*s)();
		int (T::*i)();
		long (T::*l)();
		IpAddr &(T::*ip)();
		const std::string &(T::*as)(int);
		int (T::*ai)(int);
		long (T::*al)(int);
		IpAddr &(T::*aip)(int);
		const std::string &(T::*hs)(const std::string&);
		int (T::*hi)(const std::string&);
		long (T::*hl)(const std::string&);
		IpAddr &(T::*hip)(const std::string&);
	} get_u;
	union {
		void (T::*s)(const std::string&);
		void (T::*i)(int);
		void (T::*l)(long);
		void (T::*ip)(IpAddr&);
		void (T::*as)(int, const std::string&);
		void (T::*ai)(int, int);
		void (T::*al)(int, long);
		void (T::*aip)(int, IpAddr&);
		void (T::*hs)(const std::string&, const std::string&);
		void (T::*hi)(const std::string&, int);
		void (T::*hl)(const std::string&, long);
		void (T::*hip)(const std::string&, IpAddr&);
	} set_u;
	// clear whole object (scalar, array, hash)
	void (T::*clear_all)();
	// get count of keys of an array/hash
	int (T::*count)();
	// list of keys of a hash
	std::vector<std::string> *(T::*get_all_keys)();
	// list of values of an array
	union {
		std::vector<std::string> *(T::*s)();
		std::vector<int> *(T::*i)();
		std::vector<long> *(T::*l)();
		std::vector<IpAddr> *(T::*ip)();
	} get_all_values_u;
	// set values of an array
	union {
		void (T::*s)(const std::vector<std::string>&);
		void (T::*i)(const std::vector<int>&);
		void (T::*l)(const std::vector<long>&);
		void (T::*ip)(const std::vector<IpAddr>&);
	} set_all_values_u;
	// delete hash item
	void (T::*delete_hash_item)(const std::string&);
};

inline ResourceFieldInfo::FieldType ResourceFieldInfo::getType() {
	return type;
}

template <class T>
const std::string &ResourceFieldInfoT<T>::getString(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.s ? (static_cast<T*>(resource)->*get_u.s)() : empty_string;
}

template <class T>
int ResourceFieldInfoT<T>::getInt(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.i ? (static_cast<T*>(resource)->*get_u.i)() : -1;
}

template <class T>
long ResourceFieldInfoT<T>::getLong(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.l ? (static_cast<T*>(resource)->*get_u.l)() : -1;
}

template <class T>
IpAddr &ResourceFieldInfoT<T>::getIpAddr(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.ip ? (static_cast<T*>(resource)->*get_u.ip)() : IpAddr::emptyIpAddr;
}

template <class T>
const std::string &ResourceFieldInfoT<T>::getArrayString(Resource *resource, int index) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.as ? (static_cast<T*>(resource)->*get_u.as)(index) : empty_string;
}

template <class T>
int ResourceFieldInfoT<T>::getArrayInt(Resource *resource, int index) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.ai ? (static_cast<T*>(resource)->*get_u.ai)(index) : -1;
}

template <class T>
long ResourceFieldInfoT<T>::getArrayLong(Resource *resource, int index) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.al ? (static_cast<T*>(resource)->*get_u.al)(index) : -1;
}

template <class T>
IpAddr &ResourceFieldInfoT<T>::getArrayIpAddr(Resource *resource, int index) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.aip ? (static_cast<T*>(resource)->*get_u.aip)(index) : IpAddr::emptyIpAddr;
}

template <class T>
const std::string &ResourceFieldInfoT<T>::getHashString(Resource *resource, const std::string &name) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.s ? (static_cast<T*>(resource)->*get_u.hs)(name) : empty_string;
}

template <class T>
int ResourceFieldInfoT<T>::getHashInt(Resource *resource, const std::string &name) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.i ? (static_cast<T*>(resource)->*get_u.hi)(name) : -1;
}

template <class T>
long ResourceFieldInfoT<T>::getHashLong(Resource *resource, const std::string &name) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.l ? (static_cast<T*>(resource)->*get_u.hl)(name) : -1;
}

template <class T>
IpAddr &ResourceFieldInfoT<T>::getHashIpAddr(Resource *resource, const std::string &name) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.ip ? (static_cast<T*>(resource)->*get_u.hip)(name) : IpAddr::emptyIpAddr;
}

template <class T>
void ResourceFieldInfoT<T>::setString(Resource *resource, const std::string &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.s)
		(static_cast<T*>(resource)->*set_u.s)(value);
}

template <class T>
void ResourceFieldInfoT<T>::setInt(Resource *resource, int value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.i)
		(static_cast<T*>(resource)->*set_u.i)(value);
}

template <class T>
void ResourceFieldInfoT<T>::setLong(Resource *resource, long value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.l)
		(static_cast<T*>(resource)->*set_u.l)(value);
}

template <class T>
void ResourceFieldInfoT<T>::setIpAddr(Resource *resource, IpAddr &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.ip)
		(static_cast<T*>(resource)->*set_u.ip)(value);
}

template <class T>
void ResourceFieldInfoT<T>::setArrayString(Resource *resource, int index, const std::string &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.as)
		(static_cast<T*>(resource)->*set_u.as)(index, value);
}

template <class T>
void ResourceFieldInfoT<T>::setArrayInt(Resource *resource, int index, int value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.ai)
		(static_cast<T*>(resource)->*set_u.ai)(index, value);
}

template <class T>
void ResourceFieldInfoT<T>::setArrayLong(Resource *resource, int index, long value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.al)
		(static_cast<T*>(resource)->*set_u.al)(index, value);
}

template <class T>
void ResourceFieldInfoT<T>::setArrayIpAddr(Resource *resource, int index, IpAddr &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.aip)
		(static_cast<T*>(resource)->*set_u.aip)(index, value);
}

template <class T>
void ResourceFieldInfoT<T>::setHashString(Resource *resource, const std::string &name, const std::string &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.hs)
		(static_cast<T*>(resource)->*set_u.hs)(name, value);
}

template <class T>
void ResourceFieldInfoT<T>::setHashInt(Resource *resource, const std::string &name, int value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.hi)
		(static_cast<T*>(resource)->*set_u.hi)(name, value);
}

template <class T>
void ResourceFieldInfoT<T>::setHashLong(Resource *resource, const std::string &name, long value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.hl)
		(static_cast<T*>(resource)->*set_u.hl)(name, value);
}

template <class T>
void ResourceFieldInfoT<T>::setHashIpAddr(Resource *resource, const std::string &name, IpAddr &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.hip)
		(static_cast<T*>(resource)->*set_u.hip)(name, value);
}

template <class T>
void ResourceFieldInfoT<T>::clear(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	if (clear_all)
		(static_cast<T*>(resource)->*clear_all)();
}

template <class T>
void ResourceFieldInfoT<T>::deleteHashItem(Resource *resource, const std::string &name) {
	assert(resource->getTypeId() == T::typeId);
	if (delete_hash_item)
		(static_cast<T*>(resource)->*delete_hash_item)(name);
}

template <class T>
int ResourceFieldInfoT<T>::getCount(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	return count ? (static_cast<T*>(resource)->*count)() : 0;
}

template <class T>
std::vector<std::string> *ResourceFieldInfoT<T>::getKeys(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	assert(type == HASH_STRING || type == HASH_INT || type == HASH_LONG || type == HASH_IP);
	if (get_all_keys)
		return (static_cast<T*>(resource)->*get_all_keys)();
	return new std::vector<std::string>();
}

template <class T>
std::vector<std::string> *ResourceFieldInfoT<T>::getValuesString(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	assert(type == HASH_STRING || type == ARRAY_STRING);
	if (get_all_values_u.s)
		return (static_cast<T*>(resource)->*get_all_values_u.s)();
	return new std::vector<std::string>();
}

template <class T>
std::vector<int> *ResourceFieldInfoT<T>::getValuesInt(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	assert(type == HASH_INT || type == ARRAY_INT);
	if (get_all_values_u.i)
		return (static_cast<T*>(resource)->*get_all_values_u.i)();
	return new std::vector<int>();
}

template <class T>
std::vector<long> *ResourceFieldInfoT<T>::getValuesLong(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	assert(type == HASH_LONG || type == ARRAY_LONG);
	if (get_all_values_u.l)
		return (static_cast<T*>(resource)->*get_all_values_u.l)();
	return new std::vector<long>();
}

template <class T>
std::vector<IpAddr> *ResourceFieldInfoT<T>::getValuesIpAddr(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	assert(type == HASH_IP || type == ARRAY_IP);
	if (get_all_values_u.ip)
		return (static_cast<T*>(resource)->*get_all_values_u.ip)();
	return new std::vector<IpAddr>();
}

#endif
