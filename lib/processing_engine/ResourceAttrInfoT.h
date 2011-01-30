/**
 * Class representing a resource field, so that it can be used in a general way.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_ATTR_INFO_T_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_ATTR_INFO_T_H_

#include <config.h>

#include <assert.h>
#include <string>
#include "IpAddr.h"
#include "ResourceAttrInfo.h"

template<class T>
class ResourceAttrInfoT : public ResourceAttrInfo {
public:

	ResourceAttrInfoT() {};
	~ResourceAttrInfoT() {};

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

	void InitString(const char *name, const std::string &(T::*get)(), void (T::*set)(const std::string&));
	void InitInt(const char *name, int (T::*get)(), void (T::*set)(int));
	void InitLong(const char *name, long (T::*get)(), void (T::*set)(long));
	void InitIpAddr(const char *name, IpAddr &(T::*get)(), void (T::*set)(IpAddr));

	void InitArrayString(const char *name, const std::string &get(Resource*, int), void set(Resource*, int, const std::string&), void clear(Resource*), int count(Resource*));
	void InitArrayInt(const char *name, int get(Resource*, int), void set(Resource*, int, int), void clear(Resource*), int count(Resource*));
	void InitArrayLong(const char *name, long get(Resource*, int), void set(Resource*, int, long), void clear(Resource*), int count(Resource*));
	void InitArrayIpAddr(const char *name, IpAddr &get(Resource*, int), void set(Resource*, int, IpAddr&), void clear(Resource*), int count(Resource*));

	void InitHashString(const char *name, const std::string &get(Resource*, const std::string&), void set(Resource*, const std::string, int, const std::string&), void clear(Resource*), void deleteItem(Resource*, const std::string&), int count(Resource*), std::vector<std::string> *keys(Resource*), std::vector<std::string> *values(Resource*));
	void InitHashInt(const char *name, int get(Resource*, const std::string&), void set(Resource*, const std::string, int, int), void clear(Resource*), void deleteItem(Resource*, const std::string&), int count(Resource*), std::vector<std::string> *keys(Resource*), std::vector<int> *values(Resource*));
	void InitHashLong(const char *name, long get(Resource*, const std::string&), void set(Resource*, const std::string, int, long), void clear(Resource*), void deleteItem(Resource*, const std::string&), int count(Resource*), std::vector<std::string> *keys(Resource*), std::vector<long> *values(Resource*));
	void InitHashIpAddr(const char *name, IpAddr &get(Resource*, const std::string&), void set(Resource*, const std::string, int, IpAddr&), void clear(Resource*), void deleteItem(Resource*, const std::string&), int count(Resource*), std::vector<std::string> *keys(Resource*), std::vector<IpAddr> *values(Resource*));

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

template <class T>
const std::string &ResourceAttrInfoT<T>::getString(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.s ? (static_cast<T*>(resource)->*get_u.s)() : empty_string;
}

template <class T>
int ResourceAttrInfoT<T>::getInt(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.i ? (static_cast<T*>(resource)->*get_u.i)() : -1;
}

template <class T>
long ResourceAttrInfoT<T>::getLong(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.l ? (static_cast<T*>(resource)->*get_u.l)() : -1;
}

template <class T>
IpAddr &ResourceAttrInfoT<T>::getIpAddr(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.ip ? (static_cast<T*>(resource)->*get_u.ip)() : IpAddr::emptyIpAddr;
}

template <class T>
const std::string &ResourceAttrInfoT<T>::getArrayString(Resource *resource, int index) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.as ? (static_cast<T*>(resource)->*get_u.as)(index) : empty_string;
}

template <class T>
int ResourceAttrInfoT<T>::getArrayInt(Resource *resource, int index) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.ai ? (static_cast<T*>(resource)->*get_u.ai)(index) : -1;
}

template <class T>
long ResourceAttrInfoT<T>::getArrayLong(Resource *resource, int index) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.al ? (static_cast<T*>(resource)->*get_u.al)(index) : -1;
}

template <class T>
IpAddr &ResourceAttrInfoT<T>::getArrayIpAddr(Resource *resource, int index) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.aip ? (static_cast<T*>(resource)->*get_u.aip)(index) : IpAddr::emptyIpAddr;
}

template <class T>
const std::string &ResourceAttrInfoT<T>::getHashString(Resource *resource, const std::string &name) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.s ? (static_cast<T*>(resource)->*get_u.hs)(name) : empty_string;
}

template <class T>
int ResourceAttrInfoT<T>::getHashInt(Resource *resource, const std::string &name) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.i ? (static_cast<T*>(resource)->*get_u.hi)(name) : -1;
}

template <class T>
long ResourceAttrInfoT<T>::getHashLong(Resource *resource, const std::string &name) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.l ? (static_cast<T*>(resource)->*get_u.hl)(name) : -1;
}

template <class T>
IpAddr &ResourceAttrInfoT<T>::getHashIpAddr(Resource *resource, const std::string &name) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.ip ? (static_cast<T*>(resource)->*get_u.hip)(name) : IpAddr::emptyIpAddr;
}

template <class T>
void ResourceAttrInfoT<T>::setString(Resource *resource, const std::string &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.s)
		(static_cast<T*>(resource)->*set_u.s)(value);
}

template <class T>
void ResourceAttrInfoT<T>::setInt(Resource *resource, int value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.i)
		(static_cast<T*>(resource)->*set_u.i)(value);
}

template <class T>
void ResourceAttrInfoT<T>::setLong(Resource *resource, long value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.l)
		(static_cast<T*>(resource)->*set_u.l)(value);
}

template <class T>
void ResourceAttrInfoT<T>::setIpAddr(Resource *resource, IpAddr &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.ip)
		(static_cast<T*>(resource)->*set_u.ip)(value);
}

template <class T>
void ResourceAttrInfoT<T>::setArrayString(Resource *resource, int index, const std::string &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.as)
		(static_cast<T*>(resource)->*set_u.as)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::setArrayInt(Resource *resource, int index, int value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.ai)
		(static_cast<T*>(resource)->*set_u.ai)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::setArrayLong(Resource *resource, int index, long value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.al)
		(static_cast<T*>(resource)->*set_u.al)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::setArrayIpAddr(Resource *resource, int index, IpAddr &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.aip)
		(static_cast<T*>(resource)->*set_u.aip)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::setHashString(Resource *resource, const std::string &name, const std::string &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.hs)
		(static_cast<T*>(resource)->*set_u.hs)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::setHashInt(Resource *resource, const std::string &name, int value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.hi)
		(static_cast<T*>(resource)->*set_u.hi)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::setHashLong(Resource *resource, const std::string &name, long value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.hl)
		(static_cast<T*>(resource)->*set_u.hl)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::setHashIpAddr(Resource *resource, const std::string &name, IpAddr &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.hip)
		(static_cast<T*>(resource)->*set_u.hip)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::clear(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	if (clear_all)
		(static_cast<T*>(resource)->*clear_all)();
}

template <class T>
void ResourceAttrInfoT<T>::deleteHashItem(Resource *resource, const std::string &name) {
	assert(resource->getTypeId() == T::typeId);
	if (delete_hash_item)
		(static_cast<T*>(resource)->*delete_hash_item)(name);
}

template <class T>
int ResourceAttrInfoT<T>::getCount(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	return count ? (static_cast<T*>(resource)->*count)() : 0;
}

template <class T>
std::vector<std::string> *ResourceAttrInfoT<T>::getKeys(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	assert(type == HASH_STRING || type == HASH_INT || type == HASH_LONG || type == HASH_IP);
	if (get_all_keys)
		return (static_cast<T*>(resource)->*get_all_keys)();
	return new std::vector<std::string>();
}

template <class T>
std::vector<std::string> *ResourceAttrInfoT<T>::getValuesString(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	assert(type == HASH_STRING || type == ARRAY_STRING);
	if (get_all_values_u.s)
		return (static_cast<T*>(resource)->*get_all_values_u.s)();
	return new std::vector<std::string>();
}

template <class T>
std::vector<int> *ResourceAttrInfoT<T>::getValuesInt(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	assert(type == HASH_INT || type == ARRAY_INT);
	if (get_all_values_u.i)
		return (static_cast<T*>(resource)->*get_all_values_u.i)();
	return new std::vector<int>();
}

template <class T>
std::vector<long> *ResourceAttrInfoT<T>::getValuesLong(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	assert(type == HASH_LONG || type == ARRAY_LONG);
	if (get_all_values_u.l)
		return (static_cast<T*>(resource)->*get_all_values_u.l)();
	return new std::vector<long>();
}

template <class T>
std::vector<IpAddr> *ResourceAttrInfoT<T>::getValuesIpAddr(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	assert(type == HASH_IP || type == ARRAY_IP);
	if (get_all_values_u.ip)
		return (static_cast<T*>(resource)->*get_all_values_u.ip)();
	return new std::vector<IpAddr>();
}

template <class T>
void ResourceAttrInfoT<T>::InitString(const char *name, const std::string &(T::*get)(), void (T::*set)(const std::string&)) {
	type = STRING;
	this->name = name;
	get_u.s = get;
	set_u.s = set;
}

template <class T>
void ResourceAttrInfoT<T>::InitInt(const char *name, int (T::*get)(), void (T::*set)(int)) {
	type = INT;
	this->name = name;
	get_u.i = get;
	set_u.i = set;
}

template <class T>
void ResourceAttrInfoT<T>::InitLong(const char *name, long (T::*get)(), void (T::*set)(long)) {
	type = LONG;
	this->name = name;
	get_u.l = get;
	set_u.l = set;
}

template <class T>
void ResourceAttrInfoT<T>::InitIpAddr(const char *name, IpAddr &(T::*get)(), void (T::*set)(IpAddr)) {
	type = IP;
	this->name = name;
	get_u.ip = get;
	set_u.ip = set;
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayString(const char *name, const std::string &get(Resource*, int), void set(Resource*, int, const std::string&), void clear(Resource*), int count(Resource*)) {
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayInt(const char *name, int get(Resource*, int), void set(Resource*, int, int), void clear(Resource*), int count(Resource*)) {
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayLong(const char *name, long get(Resource*, int), void set(Resource*, int, long), void clear(Resource*), int count(Resource*)) {
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayIpAddr(const char *name, IpAddr &get(Resource*, int), void set(Resource*, int, IpAddr&), void clear(Resource*), int count(Resource*)) {
}

template <class T>
void ResourceAttrInfoT<T>::InitHashString(const char *name, const std::string &get(Resource*, const std::string&), void set(Resource*, const std::string, int, const std::string&), void clear(Resource*), void deleteItem(Resource*, const std::string&), int count(Resource*), std::vector<std::string> *keys(Resource*), std::vector<std::string> *values(Resource*)) {
}

template <class T>
void ResourceAttrInfoT<T>::InitHashInt(const char *name, int get(Resource*, const std::string&), void set(Resource*, const std::string, int, int), void clear(Resource*), void deleteItem(Resource*, const std::string&), int count(Resource*), std::vector<std::string> *keys(Resource*), std::vector<int> *values(Resource*)) {
}

template <class T>
void ResourceAttrInfoT<T>::InitHashLong(const char *name, long get(Resource*, const std::string&), void set(Resource*, const std::string, int, long), void clear(Resource*), void deleteItem(Resource*, const std::string&), int count(Resource*), std::vector<std::string> *keys(Resource*), std::vector<long> *values(Resource*)) {
}

template <class T>
void ResourceAttrInfoT<T>::InitHashIpAddr(const char *name, IpAddr &get(Resource*, const std::string&), void set(Resource*, const std::string, int, IpAddr&), void clear(Resource*), void deleteItem(Resource*, const std::string&), int count(Resource*), std::vector<std::string> *keys(Resource*), std::vector<IpAddr> *values(Resource*)) {
}

#endif
