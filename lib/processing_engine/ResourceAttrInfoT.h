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

	ResourceAttrInfoT(int typeId): ResourceAttrInfo(typeId) {};
	~ResourceAttrInfoT() {};

	const std::string GetString(Resource*);
	int GetInt(Resource*);
	long GetLong(Resource*);
	IpAddr GetIpAddr(Resource*);
	const std::string GetArrayString(Resource*, int);
	int GetArrayInt(Resource*, int);
	long GetArrayLong(Resource*, int);
	IpAddr GetArrayIpAddr(Resource*, int);
	const std::string GetHashString(Resource*, const std::string&);
	int GetHashInt(Resource*, const std::string&);
	long GetHashLong(Resource*, const std::string&);
	IpAddr GetHashIpAddr(Resource*, const std::string&);

	void SetString(Resource*, const std::string&);
	void SetInt(Resource*, int);
	void SetLong(Resource*, long);
	void SetIpAddr(Resource*, IpAddr&);
	void SetArrayString(Resource*, int, const std::string&);
	void SetArrayInt(Resource*, int, int);
	void SetArrayLong(Resource*, int, long);
	void SetArrayIpAddr(Resource*, int, IpAddr&);
	void SetHashString(Resource*, const std::string&, const std::string&);
	void SetHashInt(Resource*, const std::string&, int);
	void SetHashLong(Resource*, const std::string&, long);
	void SetHashIpAddr(Resource*, const std::string&, IpAddr&);

	void Clear(Resource*);
	void DeleteHashItem(Resource*, const std::string&);

	int GetCount(Resource*);
	std::vector<std::string> *GetKeys(Resource*);
	std::vector<std::string> *GetValuesString(Resource*);
	std::vector<int> *GetValuesInt(Resource*);
	std::vector<long> *GetValuesLong(Resource*);
	std::vector<IpAddr> *GetValuesIpAddr(Resource*);

	void InitString(const char *name, const std::string (T::*get)(), void (T::*set)(const std::string&));
	void InitInt(const char *name, int (T::*get)(), void (T::*set)(int));
	void InitLong(const char *name, long (T::*get)(), void (T::*set)(long));
	void InitIpAddr(const char *name, IpAddr (T::*get)(), void (T::*set)(IpAddr&));

	void InitArrayString(const char *name, const std::string get(int), void set(int, const std::string&), void clear(), int count());
	void InitArrayInt(const char *name, int get(int), void set(int, int), void clear(), int count());
	void InitArrayLong(const char *name, long get(int), void set(int, long), void clear(), int count());
	void InitArrayIpAddr(const char *name, IpAddr get(int), void set(int, IpAddr&), void clear(), int count());

	void InitHashString(const char *name, const std::string get(const std::string&), void set(const std::string, int, const std::string&), void clear(), void deleteItem(const std::string&), int count(), std::vector<std::string> *keys(), std::vector<std::string> *values());
	void InitHashInt(const char *name, int get(const std::string&), void set(const std::string, int, int), void clear(), void deleteItem(const std::string&), int count(), std::vector<std::string> *keys(), std::vector<int> *values());
	void InitHashLong(const char *name, long get(const std::string&), void set(const std::string, int, long), void clear(), void deleteItem(const std::string&), int count(), std::vector<std::string> *keys(), std::vector<long> *values());
	void InitHashIpAddr(const char *name, IpAddr get(const std::string&), void set(const std::string, int, IpAddr&), void clear(), void deleteItem(const std::string&), int count(), std::vector<std::string> *keys(), std::vector<IpAddr> *values());

protected:
	union {
		const std::string (T::*s)();
		int (T::*i)();
		long (T::*l)();
		IpAddr (T::*ip)();
		const std::string (T::*as)(int);
		int (T::*ai)(int);
		long (T::*al)(int);
		IpAddr (T::*aip)(int);
		const std::string (T::*hs)(const std::string&);
		int (T::*hi)(const std::string&);
		long (T::*hl)(const std::string&);
		IpAddr (T::*hip)(const std::string&);
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
	std::vector<std::string> *(T::*get_hash_keys)();
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
const std::string ResourceAttrInfoT<T>::GetString(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.s ? (static_cast<T*>(resource)->*get_u.s)() : empty_string;
}

template <class T>
int ResourceAttrInfoT<T>::GetInt(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.i ? (static_cast<T*>(resource)->*get_u.i)() : -1;
}

template <class T>
long ResourceAttrInfoT<T>::GetLong(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.l ? (static_cast<T*>(resource)->*get_u.l)() : -1;
}

template <class T>
IpAddr ResourceAttrInfoT<T>::GetIpAddr(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ip ? (static_cast<T*>(resource)->*get_u.ip)() : IpAddr::emptyIpAddr;
}

template <class T>
const std::string ResourceAttrInfoT<T>::GetArrayString(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.as ? (static_cast<T*>(resource)->*get_u.as)(index) : empty_string;
}

template <class T>
int ResourceAttrInfoT<T>::GetArrayInt(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ai ? (static_cast<T*>(resource)->*get_u.ai)(index) : -1;
}

template <class T>
long ResourceAttrInfoT<T>::GetArrayLong(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.al ? (static_cast<T*>(resource)->*get_u.al)(index) : -1;
}

template <class T>
IpAddr ResourceAttrInfoT<T>::GetArrayIpAddr(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.aip ? (static_cast<T*>(resource)->*get_u.aip)(index) : IpAddr::emptyIpAddr;
}

template <class T>
const std::string ResourceAttrInfoT<T>::GetHashString(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.s ? (static_cast<T*>(resource)->*get_u.hs)(name) : empty_string;
}

template <class T>
int ResourceAttrInfoT<T>::GetHashInt(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.i ? (static_cast<T*>(resource)->*get_u.hi)(name) : -1;
}

template <class T>
long ResourceAttrInfoT<T>::GetHashLong(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.l ? (static_cast<T*>(resource)->*get_u.hl)(name) : -1;
}

template <class T>
IpAddr ResourceAttrInfoT<T>::GetHashIpAddr(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ip ? (static_cast<T*>(resource)->*get_u.hip)(name) : IpAddr::emptyIpAddr;
}

template <class T>
void ResourceAttrInfoT<T>::SetString(Resource *resource, const std::string &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.s)
		(static_cast<T*>(resource)->*set_u.s)(value);
}

template <class T>
void ResourceAttrInfoT<T>::SetInt(Resource *resource, int value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.i)
		(static_cast<T*>(resource)->*set_u.i)(value);
}

template <class T>
void ResourceAttrInfoT<T>::SetLong(Resource *resource, long value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.l)
		(static_cast<T*>(resource)->*set_u.l)(value);
}

template <class T>
void ResourceAttrInfoT<T>::SetIpAddr(Resource *resource, IpAddr &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.ip)
		(static_cast<T*>(resource)->*set_u.ip)(value);
}

template <class T>
void ResourceAttrInfoT<T>::SetArrayString(Resource *resource, int index, const std::string &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.as)
		(static_cast<T*>(resource)->*set_u.as)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetArrayInt(Resource *resource, int index, int value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.ai)
		(static_cast<T*>(resource)->*set_u.ai)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetArrayLong(Resource *resource, int index, long value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.al)
		(static_cast<T*>(resource)->*set_u.al)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetArrayIpAddr(Resource *resource, int index, IpAddr &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.aip)
		(static_cast<T*>(resource)->*set_u.aip)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetHashString(Resource *resource, const std::string &name, const std::string &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hs)
		(static_cast<T*>(resource)->*set_u.hs)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetHashInt(Resource *resource, const std::string &name, int value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hi)
		(static_cast<T*>(resource)->*set_u.hi)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetHashLong(Resource *resource, const std::string &name, long value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hl)
		(static_cast<T*>(resource)->*set_u.hl)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetHashIpAddr(Resource *resource, const std::string &name, IpAddr &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hip)
		(static_cast<T*>(resource)->*set_u.hip)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::Clear(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	if (clear_all)
		(static_cast<T*>(resource)->*clear_all)();
}

template <class T>
void ResourceAttrInfoT<T>::DeleteHashItem(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	if (delete_hash_item)
		(static_cast<T*>(resource)->*delete_hash_item)(name);
}

template <class T>
int ResourceAttrInfoT<T>::GetCount(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return count ? (static_cast<T*>(resource)->*count)() : 0;
}

template <class T>
std::vector<std::string> *ResourceAttrInfoT<T>::GetKeys(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_STRING || type == HASH_INT || type == HASH_LONG || type == HASH_IP);
	if (get_hash_keys)
		return (static_cast<T*>(resource)->*get_hash_keys)();
	return new std::vector<std::string>();
}

template <class T>
std::vector<std::string> *ResourceAttrInfoT<T>::GetValuesString(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_STRING || type == ARRAY_STRING);
	if (get_all_values_u.s)
		return (static_cast<T*>(resource)->*get_all_values_u.s)();
	return new std::vector<std::string>();
}

template <class T>
std::vector<int> *ResourceAttrInfoT<T>::GetValuesInt(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_INT || type == ARRAY_INT);
	if (get_all_values_u.i)
		return (static_cast<T*>(resource)->*get_all_values_u.i)();
	return new std::vector<int>();
}

template <class T>
std::vector<long> *ResourceAttrInfoT<T>::GetValuesLong(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_LONG || type == ARRAY_LONG);
	if (get_all_values_u.l)
		return (static_cast<T*>(resource)->*get_all_values_u.l)();
	return new std::vector<long>();
}

template <class T>
std::vector<IpAddr> *ResourceAttrInfoT<T>::GetValuesIpAddr(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_IP || type == ARRAY_IP);
	if (get_all_values_u.ip)
		return (static_cast<T*>(resource)->*get_all_values_u.ip)();
	return new std::vector<IpAddr>();
}

template <class T>
void ResourceAttrInfoT<T>::InitString(const char *name, const std::string (T::*get)(), void (T::*set)(const std::string&)) {
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
void ResourceAttrInfoT<T>::InitIpAddr(const char *name, IpAddr (T::*get)(), void (T::*set)(IpAddr&)) {
	type = IP;
	this->name = name;
	get_u.ip = get;
	set_u.ip = set;
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayString(const char *name, const std::string get(int), void set(int, const std::string&), void clear(), int count()) {
	type = ARRAY_STRING;
	this->name = name;
	get_u.as = get;
	set_u.as = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayInt(const char *name, int get(int), void set(int, int), void clear(), int count()) {
	type = ARRAY_INT;
	this->name = name;
	get_u.ai = get;
	set_u.ai = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayLong(const char *name, long get(int), void set(int, long), void clear(), int count()) {
	type = ARRAY_LONG;
	this->name = name;
	get_u.al = get;
	set_u.al = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayIpAddr(const char *name, IpAddr get(int), void set(int, IpAddr&), void clear(), int count()) {
	type = ARRAY_IP;
	this->name = name;
	get_u.aip = get;
	set_u.aip = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoT<T>::InitHashString(const char *name, const std::string get(const std::string&), void set(const std::string, int, const std::string&), void clear(), void deleteItem(const std::string&), int count(), std::vector<std::string> *keys(), std::vector<std::string> *values()) {
	type = HASH_STRING;
	this->name = name;
	get_u.hs = get;
	set_u.hs = set;
	clear_all = clear;
	delete_hash_item = deleteItem;
	this->count = count;
	get_hash_keys = keys;
	get_all_values_u.s = values;
}

template <class T>
void ResourceAttrInfoT<T>::InitHashInt(const char *name, int get(const std::string&), void set(const std::string, int, int), void clear(), void deleteItem(const std::string&), int count(), std::vector<std::string> *keys(), std::vector<int> *values()) {
	type = HASH_INT;
	this->name = name;
	get_u.hi = get;
	set_u.hi = set;
	clear_all = clear;
	delete_hash_item = deleteItem;
	this->count = count;
	get_hash_keys = keys;
	get_all_values_u.i = values;
}

template <class T>
void ResourceAttrInfoT<T>::InitHashLong(const char *name, long get(const std::string&), void set(const std::string, int, long), void clear(), void deleteItem(const std::string&), int count(), std::vector<std::string> *keys(), std::vector<long> *values()) {
	type = HASH_LONG;
	this->name = name;
	get_u.hl = get;
	set_u.hl = set;
	clear_all = clear;
	delete_hash_item = deleteItem;
	this->count = count;
	get_hash_keys = keys;
	get_all_values_u.l = values;
}

template <class T>
void ResourceAttrInfoT<T>::InitHashIpAddr(const char *name, IpAddr get(const std::string&), void set(const std::string, int, IpAddr&), void clear(), void deleteItem(const std::string&), int count(), std::vector<std::string> *keys(), std::vector<IpAddr> *values()) {
	type = HASH_IP;
	this->name = name;
	get_u.hip = get;
	set_u.hip = set;
	clear_all = clear;
	delete_hash_item = deleteItem;
	this->count = count;
	get_hash_keys = keys;
	get_all_values_u.ip = values;
}

#endif
