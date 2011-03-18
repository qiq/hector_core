/**
 * Class representing a resource field, so that it can be used in a general way.
 */

#ifndef _LIB_PROCESSING_ENGINE_RESOURCE_ATTR_INFO_P_H_
#define _LIB_PROCESSING_ENGINE_RESOURCE_ATTR_INFO_P_H_

#include <config.h>

#include <assert.h>
#include <string>
#include "IpAddr.h"
#include "ResourceAttrInfo.h"

template<class T>
class ResourceAttrInfoP : public ResourceAttrInfo {
public:

	ResourceAttrInfoP(int typeId): ResourceAttrInfo(typeId) {};
	~ResourceAttrInfoP() {};

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

	void InitString(const char *name, std::string (T::*get)(const std::string&), void (T::*set)(const std::string&, const std::string&));
	void InitInt(const char *name, int (T::*get)(const std::string&), void (T::*set)(const std::string&, int));
	void InitLong(const char *name, long (T::*get)(const std::string&), void (T::*set)(const std::string&, long));
	void InitIpAddr(const char *name, IpAddr (T::*get)(const std::string&), void (T::*set)(const std::string&, IpAddr&));

	void InitArrayString(const char *name, std::string (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, const std::string&), void (T::*clear)(const std::string&), int (T::*count)(const std::string&));
	void InitArrayInt(const char *name, int (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, int), void (T::*clear)(const std::string&), int (T::*count)(const std::string&));
	void InitArrayLong(const char *name, long (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, long), void (T::*clear)(const std::string&), int (T::*count)(const std::string&));
	void InitArrayIpAddr(const char *name, IpAddr (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, IpAddr&), void (T::*clear)(const std::string&), int (T::*count)(const std::string&));

	void InitHashString(const char *name, std::string (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, const std::string&), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<std::string> *(T::*values)(const std::string&));
	void InitHashInt(const char *name, int (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, int), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<int> *(T::*values)(const std::string&));
	void InitHashLong(const char *name, long (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, long), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<long> *(T::*values)(const std::string&));
	void InitHashIpAddr(const char *name, IpAddr (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, IpAddr&), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<IpAddr> *(T::*values)(const std::string&));

protected:
	union {
		std::string (T::*s)(const std::string&);
		int (T::*i)(const std::string&);
		long (T::*l)(const std::string&);
		IpAddr (T::*ip)(const std::string&);
		std::string (T::*as)(const std::string&, int);
		int (T::*ai)(const std::string&, int);
		long (T::*al)(const std::string&, int);
		IpAddr (T::*aip)(const std::string&, int);
		std::string (T::*hs)(const std::string&, const std::string&);
		int (T::*hi)(const std::string&, const std::string&);
		long (T::*hl)(const std::string&, const std::string&);
		IpAddr (T::*hip)(const std::string&, const std::string&);
	} get_u;
	union {
		void (T::*s)(const std::string&, const std::string&);
		void (T::*i)(const std::string&, int);
		void (T::*l)(const std::string&, long);
		void (T::*ip)(const std::string&, IpAddr&);
		void (T::*as)(const std::string&, int, const std::string&);
		void (T::*ai)(const std::string&, int, int);
		void (T::*al)(const std::string&, int, long);
		void (T::*aip)(const std::string&, int, IpAddr&);
		void (T::*hs)(const std::string&, const std::string&, const std::string&);
		void (T::*hi)(const std::string&, const std::string&, int);
		void (T::*hl)(const std::string&, const std::string&, long);
		void (T::*hip)(const std::string&, const std::string&, IpAddr&);
	} set_u;
	// clear whole object (scalar, array, hash)
	void (T::*clear_all)(const std::string&);
	// get count of keys of an array/hash
	int (T::*count)(const std::string&);
	// list of keys of a hash
	std::vector<std::string> *(T::*get_hash_keys)(const std::string&);
	// list of values of an array
	union {
		std::vector<std::string> *(T::*s)(const std::string&);
		std::vector<int> *(T::*i)(const std::string&);
		std::vector<long> *(T::*l)(const std::string&);
		std::vector<IpAddr> *(T::*ip)(const std::string&);
	} get_all_values_u;
	// set values of an array
	union {
		void (T::*s)(const std::string&, const std::vector<std::string>&);
		void (T::*i)(const std::string&, const std::vector<int>&);
		void (T::*l)(const std::string&, const std::vector<long>&);
		void (T::*ip)(const std::string&, const std::vector<IpAddr>&);
	} set_all_values_u;
	// delete hash item
	void (T::*delete_hash_item)(const std::string&, const std::string&);
};

template <class T>
const std::string ResourceAttrInfoP<T>::GetString(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.s ? (static_cast<T*>(resource)->*get_u.s)(GetName()) : empty_string;
}

template <class T>
int ResourceAttrInfoP<T>::GetInt(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.i ? (static_cast<T*>(resource)->*get_u.i)(GetName()) : -1;
}

template <class T>
long ResourceAttrInfoP<T>::GetLong(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.l ? (static_cast<T*>(resource)->*get_u.l)(GetName()) : -1;
}

template <class T>
IpAddr ResourceAttrInfoP<T>::GetIpAddr(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ip ? (static_cast<T*>(resource)->*get_u.ip)(GetName()) : IpAddr::emptyIpAddr;
}

template <class T>
const std::string ResourceAttrInfoP<T>::GetArrayString(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.as ? (static_cast<T*>(resource)->*get_u.as)(GetName(), index) : empty_string;
}

template <class T>
int ResourceAttrInfoP<T>::GetArrayInt(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ai ? (static_cast<T*>(resource)->*get_u.ai)(GetName(), index) : -1;
}

template <class T>
long ResourceAttrInfoP<T>::GetArrayLong(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.al ? (static_cast<T*>(resource)->*get_u.al)(GetName(), index) : -1;
}

template <class T>
IpAddr ResourceAttrInfoP<T>::GetArrayIpAddr(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.aip ? (static_cast<T*>(resource)->*get_u.aip)(GetName(), index) : IpAddr::emptyIpAddr;
}

template <class T>
const std::string ResourceAttrInfoP<T>::GetHashString(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.s ? (static_cast<T*>(resource)->*get_u.hs)(GetName(), name) : empty_string;
}

template <class T>
int ResourceAttrInfoP<T>::GetHashInt(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.i ? (static_cast<T*>(resource)->*get_u.hi)(GetName(), name) : -1;
}

template <class T>
long ResourceAttrInfoP<T>::GetHashLong(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.l ? (static_cast<T*>(resource)->*get_u.hl)(GetName(), name) : -1;
}

template <class T>
IpAddr ResourceAttrInfoP<T>::GetHashIpAddr(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ip ? (static_cast<T*>(resource)->*get_u.hip)(GetName(), name) : IpAddr::emptyIpAddr;
}

template <class T>
void ResourceAttrInfoP<T>::SetString(Resource *resource, const std::string &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.s)
		(static_cast<T*>(resource)->*set_u.s)(GetName(), value);
}

template <class T>
void ResourceAttrInfoP<T>::SetInt(Resource *resource, int value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.i)
		(static_cast<T*>(resource)->*set_u.i)(GetName(), value);
}

template <class T>
void ResourceAttrInfoP<T>::SetLong(Resource *resource, long value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.l)
		(static_cast<T*>(resource)->*set_u.l)(GetName(), value);
}

template <class T>
void ResourceAttrInfoP<T>::SetIpAddr(Resource *resource, IpAddr &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.ip)
		(static_cast<T*>(resource)->*set_u.ip)(GetName(), value);
}

template <class T>
void ResourceAttrInfoP<T>::SetArrayString(Resource *resource, int index, const std::string &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.as)
		(static_cast<T*>(resource)->*set_u.as)(GetName(), index, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetArrayInt(Resource *resource, int index, int value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.ai)
		(static_cast<T*>(resource)->*set_u.ai)(GetName(), index, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetArrayLong(Resource *resource, int index, long value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.al)
		(static_cast<T*>(resource)->*set_u.al)(GetName(), index, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetArrayIpAddr(Resource *resource, int index, IpAddr &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.aip)
		(static_cast<T*>(resource)->*set_u.aip)(GetName(), index, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetHashString(Resource *resource, const std::string &name, const std::string &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hs)
		(static_cast<T*>(resource)->*set_u.hs)(GetName(), name, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetHashInt(Resource *resource, const std::string &name, int value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hi)
		(static_cast<T*>(resource)->*set_u.hi)(GetName(), name, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetHashLong(Resource *resource, const std::string &name, long value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hl)
		(static_cast<T*>(resource)->*set_u.hl)(GetName(), name, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetHashIpAddr(Resource *resource, const std::string &name, IpAddr &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hip)
		(static_cast<T*>(resource)->*set_u.hip)(GetName(), name, value);
}

template <class T>
void ResourceAttrInfoP<T>::Clear(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	if (clear_all)
		(static_cast<T*>(resource)->*clear_all)(GetName());
}

template <class T>
void ResourceAttrInfoP<T>::DeleteHashItem(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	if (delete_hash_item)
		(static_cast<T*>(resource)->*delete_hash_item)(GetName(), name);
}

template <class T>
int ResourceAttrInfoP<T>::GetCount(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return count ? (static_cast<T*>(resource)->*count)(GetName()) : 0;
}

template <class T>
std::vector<std::string> *ResourceAttrInfoP<T>::GetKeys(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_STRING || type == HASH_INT || type == HASH_LONG || type == HASH_IP);
	if (get_hash_keys)
		return (static_cast<T*>(resource)->*get_hash_keys)(GetName());
	return new std::vector<std::string>();
}

template <class T>
std::vector<std::string> *ResourceAttrInfoP<T>::GetValuesString(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_STRING || type == ARRAY_STRING);
	if (get_all_values_u.s)
		return (static_cast<T*>(resource)->*get_all_values_u.s)(GetName());
	return new std::vector<std::string>();
}

template <class T>
std::vector<int> *ResourceAttrInfoP<T>::GetValuesInt(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_INT || type == ARRAY_INT);
	if (get_all_values_u.i)
		return (static_cast<T*>(resource)->*get_all_values_u.i)(GetName());
	return new std::vector<int>();
}

template <class T>
std::vector<long> *ResourceAttrInfoP<T>::GetValuesLong(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_LONG || type == ARRAY_LONG);
	if (get_all_values_u.l)
		return (static_cast<T*>(resource)->*get_all_values_u.l)(GetName());
	return new std::vector<long>();
}

template <class T>
std::vector<IpAddr> *ResourceAttrInfoP<T>::GetValuesIpAddr(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_IP || type == ARRAY_IP);
	if (get_all_values_u.ip)
		return (static_cast<T*>(resource)->*get_all_values_u.ip)(GetName());
	return new std::vector<IpAddr>();
}

template <class T>
void ResourceAttrInfoP<T>::InitString(const char *name, std::string (T::*get)(const std::string&), void (T::*set)(const std::string&, const std::string&)) {
	type = STRING;
	this->name = name;
	get_u.s = get;
	set_u.s = set;
}

template <class T>
void ResourceAttrInfoP<T>::InitInt(const char *name, int (T::*get)(const std::string&), void (T::*set)(const std::string&, int)) {
	type = INT;
	this->name = name;
	get_u.i = get;
	set_u.i = set;
}

template <class T>
void ResourceAttrInfoP<T>::InitLong(const char *name, long (T::*get)(const std::string&), void (T::*set)(const std::string&, long)) {
	type = LONG;
	this->name = name;
	get_u.l = get;
	set_u.l = set;
}

template <class T>
void ResourceAttrInfoP<T>::InitIpAddr(const char *name, IpAddr (T::*get)(const std::string&), void (T::*set)(const std::string&, IpAddr&)) {
	type = IP;
	this->name = name;
	get_u.ip = get;
	set_u.ip = set;
}

template <class T>
void ResourceAttrInfoP<T>::InitArrayString(const char *name, std::string (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, const std::string&), void (T::*clear)(const std::string&), int (T::*count)(const std::string&)) {
	type = ARRAY_STRING;
	this->name = name;
	get_u.as = get;
	set_u.as = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoP<T>::InitArrayInt(const char *name, int (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, int), void (T::*clear)(const std::string&), int (T::*count)(const std::string&)) {
	type = ARRAY_INT;
	this->name = name;
	get_u.ai = get;
	set_u.ai = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoP<T>::InitArrayLong(const char *name, long (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, long), void (T::*clear)(const std::string&), int (T::*count)(const std::string&)) {
	type = ARRAY_LONG;
	this->name = name;
	get_u.al = get;
	set_u.al = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoP<T>::InitArrayIpAddr(const char *name, IpAddr (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, IpAddr&), void (T::*clear)(const std::string&), int (T::*count)(const std::string&)) {
	type = ARRAY_IP;
	this->name = name;
	get_u.aip = get;
	set_u.aip = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoP<T>::InitHashString(const char *name, std::string (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, const std::string&), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<std::string> *(T::*values)(const std::string&)) {
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
void ResourceAttrInfoP<T>::InitHashInt(const char *name, int (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, int), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<int> *(T::*values)(const std::string&)) {
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
void ResourceAttrInfoP<T>::InitHashLong(const char *name, long (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, long), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<long> *(T::*values)(const std::string&)) {
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
void ResourceAttrInfoP<T>::InitHashIpAddr(const char *name, IpAddr (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, IpAddr&), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<IpAddr> *(T::*values)(const std::string&)) {
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
