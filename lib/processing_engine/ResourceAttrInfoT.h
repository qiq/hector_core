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

	int32_t GetInt32(Resource*);
	uint32_t GetUInt32(Resource*);
	int64_t GetInt64(Resource*);
	uint64_t GetUInt64(Resource*);
	double GetDouble(Resource*);
	const std::string GetString(Resource*);
	IpAddr GetIpAddr(Resource*);
	int32_t GetArrayInt32(Resource*, int);
	uint32_t GetArrayUInt32(Resource*, int);
	int64_t GetArrayInt64(Resource*, int);
	uint64_t GetArrayUInt64(Resource*, int);
	double GetArrayDouble(Resource*, int);
	const std::string GetArrayString(Resource*, int);
	IpAddr GetArrayIpAddr(Resource*, int);
	int32_t GetHashInt32(Resource*, const std::string&);
	uint32_t GetHashUInt32(Resource*, const std::string&);
	int64_t GetHashInt64(Resource*, const std::string&);
	uint64_t GetHashUInt64(Resource*, const std::string&);
	double GetHashDouble(Resource*, const std::string&);
	const std::string GetHashString(Resource*, const std::string&);
	IpAddr GetHashIpAddr(Resource*, const std::string&);

	void SetInt32(Resource*, int32_t);
	void SetUInt32(Resource*, uint32_t);
	void SetInt64(Resource*, int64_t);
	void SetUInt64(Resource*, uint64_t);
	void SetDouble(Resource*, double);
	void SetString(Resource*, const std::string&);
	void SetIpAddr(Resource*, IpAddr&);
	void SetArrayInt32(Resource*, int, int32_t);
	void SetArrayUInt32(Resource*, int, uint32_t);
	void SetArrayInt64(Resource*, int, int64_t);
	void SetArrayUInt64(Resource*, int, uint64_t);
	void SetArrayDouble(Resource*, int, double);
	void SetArrayString(Resource*, int, const std::string&);
	void SetArrayIpAddr(Resource*, int, IpAddr&);
	void SetHashInt32(Resource*, const std::string&, int32_t);
	void SetHashUInt32(Resource*, const std::string&, uint32_t);
	void SetHashInt64(Resource*, const std::string&, int64_t);
	void SetHashUInt64(Resource*, const std::string&, uint64_t);
	void SetHashDouble(Resource*, const std::string&, double);
	void SetHashString(Resource*, const std::string&, const std::string&);
	void SetHashIpAddr(Resource*, const std::string&, IpAddr&);

	void Clear(Resource*);
	void DeleteHashItem(Resource*, const std::string&);

	int GetCount(Resource*);
	std::vector<std::string> *GetKeys(Resource*);
	std::vector<int32_t> *GetValuesInt32(Resource*);
	std::vector<uint32_t> *GetValuesUInt32(Resource*);
	std::vector<int64_t> *GetValuesInt64(Resource*);
	std::vector<uint64_t> *GetValuesUInt64(Resource*);
	std::vector<double> *GetValuesDouble(Resource*);
	std::vector<std::string> *GetValuesString(Resource*);
	std::vector<IpAddr> *GetValuesIpAddr(Resource*);

	void InitInt32(const char *name, int32_t (T::*get)(), void (T::*set)(int32_t));
	void InitUInt32(const char *name, uint32_t (T::*get)(), void (T::*set)(uint32_t));
	void InitInt64(const char *name, int64_t (T::*get)(), void (T::*set)(int64_t));
	void InitUInt64(const char *name, uint64_t (T::*get)(), void (T::*set)(uint64_t));
	void InitDouble(const char *name, double (T::*get)(), void (T::*set)(double));
	void InitString(const char *name, const std::string (T::*get)(), void (T::*set)(const std::string&));
	void InitIpAddr(const char *name, IpAddr (T::*get)(), void (T::*set)(IpAddr&));

	void InitArrayInt32(const char *name, int32_t (T::*get)(int), void (T::*set)(int, int32_t), void (T::*clear)(), int (T::*count)());
	void InitArrayUInt32(const char *name, uint32_t (T::*get)(int), void (T::*set)(int, uint32_t), void (T::*clear)(), int (T::*count)());
	void InitArrayInt64(const char *name, int64_t (T::*get)(int), void (T::*set)(int, int64_t), void (T::*clear)(), int (T::*count)());
	void InitArrayUInt64(const char *name, uint64_t (T::*get)(int), void (T::*set)(int, uint64_t), void (T::*clear)(), int (T::*count)());
	void InitArrayDouble(const char *name, double (T::*get)(int), void (T::*set)(int, double), void (T::*clear)(), int (T::*count)());
	void InitArrayString(const char *name, const std::string (T::*get)(int), void (T::*set)(int, const std::string&), void (T::*clear)(), int (T::*count)());
	void InitArrayIpAddr(const char *name, IpAddr (T::*get)(int), void (T::*set)(int, IpAddr&), void (T::*clear)(), int (T::*count)());

	void InitHashInt32(const char *name, int32_t (T::*get)(const std::string&), void (T::*set)(const std::string&, int32_t), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<int32_t> *(T::*values)());
	void InitHashUInt32(const char *name, uint32_t (T::*get)(const std::string&), void (T::*set)(const std::string&, uint32_t), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<uint32_t> *(T::*values)());
	void InitHashInt64(const char *name, int64_t (T::*get)(const std::string&), void (T::*set)(const std::string&, int64_t), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<int64_t> *(T::*values)());
	void InitHashUInt64(const char *name, uint64_t (T::*get)(const std::string&), void (T::*set)(const std::string&, uint64_t), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<uint64_t> *(T::*values)());
	void InitHashDouble(const char *name, double (T::*get)(const std::string&), void (T::*set)(const std::string&, double), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<double> *(T::*values)());
	void InitHashString(const char *name, const std::string (T::*get)(const std::string&), void (T::*set)(const std::string&, const std::string&), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<std::string> *(T::*values)());
	void InitHashIpAddr(const char *name, IpAddr (T::*get)(const std::string&), void (T::*set)(const std::string&, IpAddr&), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<IpAddr> *(T::*values)());

protected:
	union {
		int32_t (T::*i32)();
		uint32_t (T::*u32)();
		int64_t (T::*i64)();
		uint64_t (T::*u64)();
		double (T::*d)();
		const std::string (T::*s)();
		IpAddr (T::*ip)();
		int32_t (T::*ai32)(int);
		uint32_t (T::*au32)(int);
		int64_t (T::*ai64)(int);
		uint64_t (T::*au64)(int);
		double (T::*ad)(int);
		const std::string (T::*as)(int);
		IpAddr (T::*aip)(int);
		int32_t (T::*hi32)(const std::string&);
		uint32_t (T::*hu32)(const std::string&);
		int64_t (T::*hi64)(const std::string&);
		uint64_t (T::*hu64)(const std::string&);
		double (T::*hd)(const std::string&);
		const std::string (T::*hs)(const std::string&);
		IpAddr (T::*hip)(const std::string&);
	} get_u;
	union {
		void (T::*i32)(int32_t);
		void (T::*u32)(uint32_t);
		void (T::*i64)(int64_t);
		void (T::*u64)(uint64_t);
		void (T::*d)(double);
		void (T::*s)(const std::string&);
		void (T::*ip)(IpAddr&);
		void (T::*ai32)(int, int32_t);
		void (T::*au32)(int, uint32_t);
		void (T::*ai64)(int, int64_t);
		void (T::*au64)(int, uint64_t);
		void (T::*ad)(int, double);
		void (T::*as)(int, const std::string&);
		void (T::*aip)(int, IpAddr&);
		void (T::*hi32)(const std::string&, int32_t);
		void (T::*hu32)(const std::string&, uint32_t);
		void (T::*hi64)(const std::string&, int64_t);
		void (T::*hu64)(const std::string&, uint64_t);
		void (T::*hd)(const std::string&, double);
		void (T::*hs)(const std::string&, const std::string&);
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
		std::vector<int32_t> *(T::*i32)();
		std::vector<uint32_t> *(T::*u32)();
		std::vector<int64_t> *(T::*i64)();
		std::vector<uint64_t> *(T::*u64)();
		std::vector<double> *(T::*d)();
		std::vector<std::string> *(T::*s)();
		std::vector<IpAddr> *(T::*ip)();
	} get_all_values_u;
	// set values of an array
	union {
		void (T::*i32)(const std::vector<int32_t>&);
		void (T::*u32)(const std::vector<uint32_t>&);
		void (T::*i64)(const std::vector<int64_t>&);
		void (T::*u64)(const std::vector<uint64_t>&);
		void (T::*d)(const std::vector<double>&);
		void (T::*s)(const std::vector<std::string>&);
		void (T::*ip)(const std::vector<IpAddr>&);
	} set_all_values_u;
	// delete hash item
	void (T::*delete_hash_item)(const std::string&);
};

template <class T>
int32_t ResourceAttrInfoT<T>::GetInt32(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.i32 ? (static_cast<T*>(resource)->*get_u.i32)() : -1;
}

template <class T>
uint32_t ResourceAttrInfoT<T>::GetUInt32(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.u32 ? (static_cast<T*>(resource)->*get_u.u32)() : -1;
}

template <class T>
int64_t ResourceAttrInfoT<T>::GetInt64(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.i64 ? (static_cast<T*>(resource)->*get_u.i64)() : -1;
}

template <class T>
uint64_t ResourceAttrInfoT<T>::GetUInt64(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.u64 ? (static_cast<T*>(resource)->*get_u.u64)() : -1;
}

template <class T>
double ResourceAttrInfoT<T>::GetDouble(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.d ? (static_cast<T*>(resource)->*get_u.d)() : -1;
}

template <class T>
const std::string ResourceAttrInfoT<T>::GetString(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.s ? (static_cast<T*>(resource)->*get_u.s)() : empty_string;
}

template <class T>
IpAddr ResourceAttrInfoT<T>::GetIpAddr(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ip ? (static_cast<T*>(resource)->*get_u.ip)() : IpAddr::emptyIpAddr;
}

template <class T>
int32_t ResourceAttrInfoT<T>::GetArrayInt32(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ai32 ? (static_cast<T*>(resource)->*get_u.ai32)(index) : -1;
}

template <class T>
uint32_t ResourceAttrInfoT<T>::GetArrayUInt32(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.au32 ? (static_cast<T*>(resource)->*get_u.au32)(index) : -1;
}

template <class T>
int64_t ResourceAttrInfoT<T>::GetArrayInt64(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ai64 ? (static_cast<T*>(resource)->*get_u.ai64)(index) : -1;
}

template <class T>
uint64_t ResourceAttrInfoT<T>::GetArrayUInt64(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.au64 ? (static_cast<T*>(resource)->*get_u.au64)(index) : -1;
}

template <class T>
double ResourceAttrInfoT<T>::GetArrayDouble(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ad ? (static_cast<T*>(resource)->*get_u.ad)(index) : -1;
}

template <class T>
const std::string ResourceAttrInfoT<T>::GetArrayString(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.as ? (static_cast<T*>(resource)->*get_u.as)(index) : empty_string;
}

template <class T>
IpAddr ResourceAttrInfoT<T>::GetArrayIpAddr(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.aip ? (static_cast<T*>(resource)->*get_u.aip)(index) : IpAddr::emptyIpAddr;
}

template <class T>
int32_t ResourceAttrInfoT<T>::GetHashInt32(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hi32 ? (static_cast<T*>(resource)->*get_u.hi32)(name) : -1;
}

template <class T>
uint32_t ResourceAttrInfoT<T>::GetHashUInt32(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hu32 ? (static_cast<T*>(resource)->*get_u.hu32)(name) : -1;
}

template <class T>
int64_t ResourceAttrInfoT<T>::GetHashInt64(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hi64 ? (static_cast<T*>(resource)->*get_u.hi64)(name) : -1;
}

template <class T>
uint64_t ResourceAttrInfoT<T>::GetHashUInt64(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hu64 ? (static_cast<T*>(resource)->*get_u.hu64)(name) : -1;
}

template <class T>
double ResourceAttrInfoT<T>::GetHashDouble(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hd ? (static_cast<T*>(resource)->*get_u.hd)(name) : -1;
}

template <class T>
const std::string ResourceAttrInfoT<T>::GetHashString(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hs ? (static_cast<T*>(resource)->*get_u.hs)(name) : empty_string;
}

template <class T>
IpAddr ResourceAttrInfoT<T>::GetHashIpAddr(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hip ? (static_cast<T*>(resource)->*get_u.hip)(name) : IpAddr::emptyIpAddr;
}

template <class T>
void ResourceAttrInfoT<T>::SetInt32(Resource *resource, int32_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.i32)
		(static_cast<T*>(resource)->*set_u.i32)(value);
}

template <class T>
void ResourceAttrInfoT<T>::SetUInt32(Resource *resource, uint32_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.u32)
		(static_cast<T*>(resource)->*set_u.u32)(value);
}

template <class T>
void ResourceAttrInfoT<T>::SetInt64(Resource *resource, int64_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.i64)
		(static_cast<T*>(resource)->*set_u.i64)(value);
}

template <class T>
void ResourceAttrInfoT<T>::SetUInt64(Resource *resource, uint64_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.u64)
		(static_cast<T*>(resource)->*set_u.u64)(value);
}

template <class T>
void ResourceAttrInfoT<T>::SetDouble(Resource *resource, double value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.d)
		(static_cast<T*>(resource)->*set_u.d)(value);
}

template <class T>
void ResourceAttrInfoT<T>::SetString(Resource *resource, const std::string &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.s)
		(static_cast<T*>(resource)->*set_u.s)(value);
}

template <class T>
void ResourceAttrInfoT<T>::SetIpAddr(Resource *resource, IpAddr &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.ip)
		(static_cast<T*>(resource)->*set_u.ip)(value);
}

template <class T>
void ResourceAttrInfoT<T>::SetArrayInt32(Resource *resource, int index, int32_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.ai32)
		(static_cast<T*>(resource)->*set_u.ai32)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetArrayUInt32(Resource *resource, int index, uint32_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.au32)
		(static_cast<T*>(resource)->*set_u.au32)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetArrayInt64(Resource *resource, int index, int64_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.ai64)
		(static_cast<T*>(resource)->*set_u.ai64)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetArrayUInt64(Resource *resource, int index, uint64_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.au64)
		(static_cast<T*>(resource)->*set_u.au64)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetArrayDouble(Resource *resource, int index, double value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.ad)
		(static_cast<T*>(resource)->*set_u.ad)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetArrayString(Resource *resource, int index, const std::string &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.as)
		(static_cast<T*>(resource)->*set_u.as)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetArrayIpAddr(Resource *resource, int index, IpAddr &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.aip)
		(static_cast<T*>(resource)->*set_u.aip)(index, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetHashInt32(Resource *resource, const std::string &name, int32_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hi32)
		(static_cast<T*>(resource)->*set_u.hi32)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetHashUInt32(Resource *resource, const std::string &name, uint32_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hu32)
		(static_cast<T*>(resource)->*set_u.hu32)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetHashInt64(Resource *resource, const std::string &name, int64_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hi64)
		(static_cast<T*>(resource)->*set_u.hi64)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetHashUInt64(Resource *resource, const std::string &name, uint64_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hu64)
		(static_cast<T*>(resource)->*set_u.hu64)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetHashDouble(Resource *resource, const std::string &name, double value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hd)
		(static_cast<T*>(resource)->*set_u.hd)(name, value);
}

template <class T>
void ResourceAttrInfoT<T>::SetHashString(Resource *resource, const std::string &name, const std::string &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hs)
		(static_cast<T*>(resource)->*set_u.hs)(name, value);
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
	assert(type == HASH_STRING || type == HASH_INT32 || type == HASH_UINT32 || type == HASH_INT64 || type == HASH_UINT64 || type == HASH_IP);
	if (get_hash_keys)
		return (static_cast<T*>(resource)->*get_hash_keys)();
	return new std::vector<std::string>();
}

template <class T>
std::vector<int32_t> *ResourceAttrInfoT<T>::GetValuesInt32(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_INT32 || type == ARRAY_INT32);
	if (get_all_values_u.i32)
		return (static_cast<T*>(resource)->*get_all_values_u.i32)();
	return new std::vector<int32_t>();
}

template <class T>
std::vector<uint32_t> *ResourceAttrInfoT<T>::GetValuesUInt32(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_UINT32 || type == ARRAY_UINT32);
	if (get_all_values_u.u32)
		return (static_cast<T*>(resource)->*get_all_values_u.u32)();
	return new std::vector<uint32_t>();
}

template <class T>
std::vector<int64_t> *ResourceAttrInfoT<T>::GetValuesInt64(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_INT64 || type == ARRAY_INT64);
	if (get_all_values_u.i64)
		return (static_cast<T*>(resource)->*get_all_values_u.i64)();
	return new std::vector<int64_t>();
}

template <class T>
std::vector<uint64_t> *ResourceAttrInfoT<T>::GetValuesUInt64(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_UINT64 || type == ARRAY_UINT64);
	if (get_all_values_u.u64)
		return (static_cast<T*>(resource)->*get_all_values_u.u64)();
	return new std::vector<uint64_t>();
}

template <class T>
std::vector<double> *ResourceAttrInfoT<T>::GetValuesDouble(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_UINT64 || type == ARRAY_UINT64);
	if (get_all_values_u.d)
		return (static_cast<T*>(resource)->*get_all_values_u.d)();
	return new std::vector<double>();
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
std::vector<IpAddr> *ResourceAttrInfoT<T>::GetValuesIpAddr(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_IP || type == ARRAY_IP);
	if (get_all_values_u.ip)
		return (static_cast<T*>(resource)->*get_all_values_u.ip)();
	return new std::vector<IpAddr>();
}

template <class T>
void ResourceAttrInfoT<T>::InitInt32(const char *name, int32_t (T::*get)(), void (T::*set)(int32_t)) {
	type = INT32;
	this->name = name;
	get_u.i32 = get;
	set_u.i32 = set;
}

template <class T>
void ResourceAttrInfoT<T>::InitUInt32(const char *name, uint32_t (T::*get)(), void (T::*set)(uint32_t)) {
	type = UINT32;
	this->name = name;
	get_u.u32 = get;
	set_u.u32 = set;
}

template <class T>
void ResourceAttrInfoT<T>::InitInt64(const char *name, int64_t (T::*get)(), void (T::*set)(int64_t)) {
	type = INT64;
	this->name = name;
	get_u.i64 = get;
	set_u.i64 = set;
}

template <class T>
void ResourceAttrInfoT<T>::InitUInt64(const char *name, uint64_t (T::*get)(), void (T::*set)(uint64_t)) {
	type = UINT64;
	this->name = name;
	get_u.u64 = get;
	set_u.u64 = set;
}

template <class T>
void ResourceAttrInfoT<T>::InitDouble(const char *name, double (T::*get)(), void (T::*set)(double)) {
	type = UINT64;
	this->name = name;
	get_u.d = get;
	set_u.d = set;
}

template <class T>
void ResourceAttrInfoT<T>::InitString(const char *name, const std::string (T::*get)(), void (T::*set)(const std::string&)) {
	type = STRING;
	this->name = name;
	get_u.s = get;
	set_u.s = set;
}

template <class T>
void ResourceAttrInfoT<T>::InitIpAddr(const char *name, IpAddr (T::*get)(), void (T::*set)(IpAddr&)) {
	type = IP;
	this->name = name;
	get_u.ip = get;
	set_u.ip = set;
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayInt32(const char *name, int32_t (T::*get)(int), void (T::*set)(int, int32_t), void (T::*clear)(), int (T::*count)()) {
	type = ARRAY_INT32;
	this->name = name;
	get_u.ai32 = get;
	set_u.ai32 = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayUInt32(const char *name, uint32_t (T::*get)(int), void (T::*set)(int, uint32_t), void (T::*clear)(), int (T::*count)()) {
	type = ARRAY_UINT32;
	this->name = name;
	get_u.au32 = get;
	set_u.au32 = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayInt64(const char *name, int64_t (T::*get)(int), void (T::*set)(int, int64_t), void (T::*clear)(), int (T::*count)()) {
	type = ARRAY_INT64;
	this->name = name;
	get_u.ai64 = get;
	set_u.ai64 = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayUInt64(const char *name, uint64_t (T::*get)(int), void (T::*set)(int, uint64_t), void (T::*clear)(), int (T::*count)()) {
	type = ARRAY_UINT64;
	this->name = name;
	get_u.au64 = get;
	set_u.au64 = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayDouble(const char *name, double (T::*get)(int), void (T::*set)(int, double), void (T::*clear)(), int (T::*count)()) {
	type = ARRAY_UINT64;
	this->name = name;
	get_u.ad = get;
	set_u.ad = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayString(const char *name, const std::string (T::*get)(int), void (T::*set)(int, const std::string&), void (T::*clear)(), int (T::*count)()) {
	type = ARRAY_STRING;
	this->name = name;
	get_u.as = get;
	set_u.as = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoT<T>::InitArrayIpAddr(const char *name, IpAddr (T::*get)(int), void (T::*set)(int, IpAddr&), void (T::*clear)(), int (T::*count)()) {
	type = ARRAY_IP;
	this->name = name;
	get_u.aip = get;
	set_u.aip = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoT<T>::InitHashInt32(const char *name, int32_t (T::*get)(const std::string&), void (T::*set)(const std::string&, int32_t), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<int32_t> *(T::*values)()) {
	type = HASH_INT32;
	this->name = name;
	get_u.hi32 = get;
	set_u.hi32 = set;
	clear_all = clear;
	delete_hash_item = deleteItem;
	this->count = count;
	get_hash_keys = keys;
	get_all_values_u.i32 = values;
}

template <class T>
void ResourceAttrInfoT<T>::InitHashUInt32(const char *name, uint32_t (T::*get)(const std::string&), void (T::*set)(const std::string&, uint32_t), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<uint32_t> *(T::*values)()) {
	type = HASH_UINT32;
	this->name = name;
	get_u.hu32 = get;
	set_u.hu32 = set;
	clear_all = clear;
	delete_hash_item = deleteItem;
	this->count = count;
	get_hash_keys = keys;
	get_all_values_u.u32 = values;
}

template <class T>
void ResourceAttrInfoT<T>::InitHashInt64(const char *name, int64_t (T::*get)(const std::string&), void (T::*set)(const std::string&, int64_t), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<int64_t> *(T::*values)()) {
	type = HASH_INT64;
	this->name = name;
	get_u.hi64 = get;
	set_u.hi64 = set;
	clear_all = clear;
	delete_hash_item = deleteItem;
	this->count = count;
	get_hash_keys = keys;
	get_all_values_u.i64 = values;
}

template <class T>
void ResourceAttrInfoT<T>::InitHashUInt64(const char *name, uint64_t (T::*get)(const std::string&), void (T::*set)(const std::string&, uint64_t), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<uint64_t> *(T::*values)()) {
	type = HASH_UINT64;
	this->name = name;
	get_u.hu64 = get;
	set_u.hu64 = set;
	clear_all = clear;
	delete_hash_item = deleteItem;
	this->count = count;
	get_hash_keys = keys;
	get_all_values_u.u64 = values;
}

template <class T>
void ResourceAttrInfoT<T>::InitHashDouble(const char *name, double (T::*get)(const std::string&), void (T::*set)(const std::string&, double), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<double> *(T::*values)()) {
	type = HASH_UINT64;
	this->name = name;
	get_u.hd = get;
	set_u.hd = set;
	clear_all = clear;
	delete_hash_item = deleteItem;
	this->count = count;
	get_hash_keys = keys;
	get_all_values_u.d = values;
}

template <class T>
void ResourceAttrInfoT<T>::InitHashString(const char *name, const std::string (T::*get)(const std::string&), void (T::*set)(const std::string&, const std::string&), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<std::string> *(T::*values)()) {
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
void ResourceAttrInfoT<T>::InitHashIpAddr(const char *name, IpAddr (T::*get)(const std::string&), void (T::*set)(const std::string&, IpAddr&), void (T::*clear)(), void (T::*deleteItem)(const std::string&), int (T::*count)(), std::vector<std::string> *(T::*keys)(), std::vector<IpAddr> *(T::*values)()) {
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
