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
	int32_t GetInt32(Resource*);
	uint32_t GetUInt32(Resource*);
	int64_t GetInt64(Resource*);
	uint64_t GetUInt64(Resource*);
	double GetDouble(Resource*);
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

	void InitInt32(const char *name, int32_t (T::*get)(const std::string&), void (T::*set)(const std::string&, int32_t));
	void InitUInt32(const char *name, uint32_t (T::*get)(const std::string&), void (T::*set)(const std::string&, uint32_t));
	void InitInt64(const char *name, int64_t (T::*get)(const std::string&), void (T::*set)(const std::string&, int64_t));
	void InitUInt64(const char *name, uint64_t (T::*get)(const std::string&), void (T::*set)(const std::string&, uint64_t));
	void InitDouble(const char *name, double (T::*get)(const std::string&), void (T::*set)(const std::string&, double));
	void InitString(const char *name, std::string (T::*get)(const std::string&), void (T::*set)(const std::string&, const std::string&));
	void InitIpAddr(const char *name, IpAddr (T::*get)(const std::string&), void (T::*set)(const std::string&, IpAddr&));

	void InitArrayInt32(const char *name, int32_t (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, int32_t), void (T::*clear)(const std::string&), int (T::*count)(const std::string&));
	void InitArrayUInt32(const char *name, uint32_t (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, uint32_t), void (T::*clear)(const std::string&), int (T::*count)(const std::string&));
	void InitArrayInt64(const char *name, int64_t (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, int64_t), void (T::*clear)(const std::string&), int (T::*count)(const std::string&));
	void InitArrayUInt64(const char *name, uint64_t (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, uint64_t), void (T::*clear)(const std::string&), int (T::*count)(const std::string&));
	void InitArrayDouble(const char *name, double (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, double), void (T::*clear)(const std::string&), int (T::*count)(const std::string&));
	void InitArrayString(const char *name, std::string (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, const std::string&), void (T::*clear)(const std::string&), int (T::*count)(const std::string&));
	void InitArrayIpAddr(const char *name, IpAddr (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, IpAddr&), void (T::*clear)(const std::string&), int (T::*count)(const std::string&));

	void InitHashInt32(const char *name, int32_t (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, int32_t), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<int32_t> *(T::*values)(const std::string&));
	void InitHashUInt32(const char *name, uint32_t (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, uint32_t), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<uint32_t> *(T::*values)(const std::string&));
	void InitHashInt64(const char *name, int64_t (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, int64_t), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<int64_t> *(T::*values)(const std::string&));
	void InitHashUInt64(const char *name, uint64_t (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, uint64_t), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<uint64_t> *(T::*values)(const std::string&));
	void InitHashDouble(const char *name, double (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, double), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<double> *(T::*values)(const std::string&));
	void InitHashString(const char *name, std::string (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, const std::string&), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<std::string> *(T::*values)(const std::string&));
	void InitHashIpAddr(const char *name, IpAddr (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, IpAddr&), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<IpAddr> *(T::*values)(const std::string&));

protected:
	union {
		int32_t (T::*i32)(const std::string&);
		uint32_t (T::*u32)(const std::string&);
		int64_t (T::*i64)(const std::string&);
		uint64_t (T::*u64)(const std::string&);
		double (T::*d)(const std::string&);
		std::string (T::*s)(const std::string&);
		IpAddr (T::*ip)(const std::string&);
		int32_t (T::*ai32)(const std::string&, int);
		uint32_t (T::*au32)(const std::string&, int);
		int64_t (T::*ai64)(const std::string&, int);
		uint64_t (T::*au64)(const std::string&, int);
		double (T::*ad)(const std::string&, int);
		std::string (T::*as)(const std::string&, int);
		IpAddr (T::*aip)(const std::string&, int);
		int32_t (T::*hi32)(const std::string&, const std::string&);
		uint32_t (T::*hu32)(const std::string&, const std::string&);
		int64_t (T::*hi64)(const std::string&, const std::string&);
		uint64_t (T::*hu64)(const std::string&, const std::string&);
		double (T::*hd)(const std::string&, const std::string&);
		std::string (T::*hs)(const std::string&, const std::string&);
		IpAddr (T::*hip)(const std::string&, const std::string&);
	} get_u;
	union {
		void (T::*i32)(const std::string&, int32_t);
		void (T::*u32)(const std::string&, uint32_t);
		void (T::*i64)(const std::string&, int64_t);
		void (T::*u64)(const std::string&, uint64_t);
		void (T::*d)(const std::string&, double);
		void (T::*s)(const std::string&, const std::string&);
		void (T::*ip)(const std::string&, IpAddr&);
		void (T::*ai32)(const std::string&, int, int32_t);
		void (T::*au32)(const std::string&, int, uint32_t);
		void (T::*ai64)(const std::string&, int, int64_t);
		void (T::*au64)(const std::string&, int, uint64_t);
		void (T::*ad)(const std::string&, int, double);
		void (T::*as)(const std::string&, int, const std::string&);
		void (T::*aip)(const std::string&, int, IpAddr&);
		void (T::*hi32)(const std::string&, const std::string&, int32_t);
		void (T::*hu32)(const std::string&, const std::string&, uint32_t);
		void (T::*hi64)(const std::string&, const std::string&, int64_t);
		void (T::*hu64)(const std::string&, const std::string&, uint64_t);
		void (T::*hd)(const std::string&, const std::string&, double);
		void (T::*hs)(const std::string&, const std::string&, const std::string&);
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
		std::vector<int32_t> *(T::*i32)(const std::string&);
		std::vector<uint32_t> *(T::*u32)(const std::string&);
		std::vector<int64_t> *(T::*i64)(const std::string&);
		std::vector<uint64_t> *(T::*u64)(const std::string&);
		std::vector<double> *(T::*d)(const std::string&);
		std::vector<std::string> *(T::*s)(const std::string&);
		std::vector<IpAddr> *(T::*ip)(const std::string&);
	} get_all_values_u;
	// set values of an array
	union {
		void (T::*s)(const std::string&, const std::vector<std::string>&);
		void (T::*i32)(const std::string&, const std::vector<int32_t>&);
		void (T::*u32)(const std::string&, const std::vector<uint32_t>&);
		void (T::*i64)(const std::string&, const std::vector<int64_t>&);
		void (T::*u64)(const std::string&, const std::vector<uint64_t>&);
		void (T::*d)(const std::string&, const std::vector<double>&);
		void (T::*ip)(const std::string&, const std::vector<IpAddr>&);
	} set_all_values_u;
	// delete hash item
	void (T::*delete_hash_item)(const std::string&, const std::string&);
};

template <class T>
int32_t ResourceAttrInfoP<T>::GetInt32(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.i32 ? (static_cast<T*>(resource)->*get_u.i32)(GetName()) : -1;
}

template <class T>
uint32_t ResourceAttrInfoP<T>::GetUInt32(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.u32 ? (static_cast<T*>(resource)->*get_u.u32)(GetName()) : -1;
}

template <class T>
int64_t ResourceAttrInfoP<T>::GetInt64(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.i64 ? (static_cast<T*>(resource)->*get_u.i64)(GetName()) : -1;
}

template <class T>
uint64_t ResourceAttrInfoP<T>::GetUInt64(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.u64 ? (static_cast<T*>(resource)->*get_u.u64)(GetName()) : -1;
}

template <class T>
double ResourceAttrInfoP<T>::GetDouble(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.d ? (static_cast<T*>(resource)->*get_u.d)(GetName()) : -1;
}

template <class T>
const std::string ResourceAttrInfoP<T>::GetString(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.s ? (static_cast<T*>(resource)->*get_u.s)(GetName()) : empty_string;
}

template <class T>
IpAddr ResourceAttrInfoP<T>::GetIpAddr(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ip ? (static_cast<T*>(resource)->*get_u.ip)(GetName()) : IpAddr::emptyIpAddr;
}

template <class T>
int32_t ResourceAttrInfoP<T>::GetArrayInt32(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ai32 ? (static_cast<T*>(resource)->*get_u.ai32)(GetName(), index) : -1;
}

template <class T>
uint32_t ResourceAttrInfoP<T>::GetArrayUInt32(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.au32 ? (static_cast<T*>(resource)->*get_u.au32)(GetName(), index) : -1;
}

template <class T>
int64_t ResourceAttrInfoP<T>::GetArrayInt64(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ai64 ? (static_cast<T*>(resource)->*get_u.ai64)(GetName(), index) : -1;
}

template <class T>
uint64_t ResourceAttrInfoP<T>::GetArrayUInt64(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.au64 ? (static_cast<T*>(resource)->*get_u.au64)(GetName(), index) : -1;
}

template <class T>
double ResourceAttrInfoP<T>::GetArrayDouble(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.ad ? (static_cast<T*>(resource)->*get_u.ad)(GetName(), index) : -1;
}

template <class T>
const std::string ResourceAttrInfoP<T>::GetArrayString(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.as ? (static_cast<T*>(resource)->*get_u.as)(GetName(), index) : empty_string;
}

template <class T>
IpAddr ResourceAttrInfoP<T>::GetArrayIpAddr(Resource *resource, int index) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.aip ? (static_cast<T*>(resource)->*get_u.aip)(GetName(), index) : IpAddr::emptyIpAddr;
}

template <class T>
int32_t ResourceAttrInfoP<T>::GetHashInt32(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hi32 ? (static_cast<T*>(resource)->*get_u.hi32)(GetName(), name) : -1;
}

template <class T>
uint32_t ResourceAttrInfoP<T>::GetHashUInt32(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hu32 ? (static_cast<T*>(resource)->*get_u.hu32)(GetName(), name) : -1;
}

template <class T>
int64_t ResourceAttrInfoP<T>::GetHashInt64(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hi64 ? (static_cast<T*>(resource)->*get_u.hi64)(GetName(), name) : -1;
}

template <class T>
uint64_t ResourceAttrInfoP<T>::GetHashUInt64(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hu64 ? (static_cast<T*>(resource)->*get_u.hu64)(GetName(), name) : -1;
}

template <class T>
double ResourceAttrInfoP<T>::GetHashDouble(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hd ? (static_cast<T*>(resource)->*get_u.hd)(GetName(), name) : -1;
}

template <class T>
const std::string ResourceAttrInfoP<T>::GetHashString(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hs ? (static_cast<T*>(resource)->*get_u.hs)(GetName(), name) : empty_string;
}

template <class T>
IpAddr ResourceAttrInfoP<T>::GetHashIpAddr(Resource *resource, const std::string &name) {
	assert(resource->GetTypeId() == GetTypeId());
	return get_u.hip ? (static_cast<T*>(resource)->*get_u.hip)(GetName(), name) : IpAddr::emptyIpAddr;
}

template <class T>
void ResourceAttrInfoP<T>::SetInt32(Resource *resource, int32_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.i32)
		(static_cast<T*>(resource)->*set_u.i32)(GetName(), value);
}

template <class T>
void ResourceAttrInfoP<T>::SetUInt32(Resource *resource, uint32_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.u32)
		(static_cast<T*>(resource)->*set_u.u32)(GetName(), value);
}

template <class T>
void ResourceAttrInfoP<T>::SetInt64(Resource *resource, int64_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.i64)
		(static_cast<T*>(resource)->*set_u.i64)(GetName(), value);
}

template <class T>
void ResourceAttrInfoP<T>::SetUInt64(Resource *resource, uint64_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.u64)
		(static_cast<T*>(resource)->*set_u.u64)(GetName(), value);
}

template <class T>
void ResourceAttrInfoP<T>::SetDouble(Resource *resource, double value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.d)
		(static_cast<T*>(resource)->*set_u.d)(GetName(), value);
}

template <class T>
void ResourceAttrInfoP<T>::SetString(Resource *resource, const std::string &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.s)
		(static_cast<T*>(resource)->*set_u.s)(GetName(), value);
}

template <class T>
void ResourceAttrInfoP<T>::SetIpAddr(Resource *resource, IpAddr &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.ip)
		(static_cast<T*>(resource)->*set_u.ip)(GetName(), value);
}

template <class T>
void ResourceAttrInfoP<T>::SetArrayInt32(Resource *resource, int index, int32_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.ai32)
		(static_cast<T*>(resource)->*set_u.ai32)(GetName(), index, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetArrayUInt32(Resource *resource, int index, uint32_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.au32)
		(static_cast<T*>(resource)->*set_u.au32)(GetName(), index, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetArrayInt64(Resource *resource, int index, int64_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.ai64)
		(static_cast<T*>(resource)->*set_u.ai64)(GetName(), index, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetArrayUInt64(Resource *resource, int index, uint64_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.au64)
		(static_cast<T*>(resource)->*set_u.au64)(GetName(), index, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetArrayDouble(Resource *resource, int index, double value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.ad)
		(static_cast<T*>(resource)->*set_u.ad)(GetName(), index, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetArrayString(Resource *resource, int index, const std::string &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.as)
		(static_cast<T*>(resource)->*set_u.as)(GetName(), index, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetArrayIpAddr(Resource *resource, int index, IpAddr &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.aip)
		(static_cast<T*>(resource)->*set_u.aip)(GetName(), index, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetHashInt32(Resource *resource, const std::string &name, int32_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hi32)
		(static_cast<T*>(resource)->*set_u.hi32)(GetName(), name, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetHashUInt32(Resource *resource, const std::string &name, uint32_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hu32)
		(static_cast<T*>(resource)->*set_u.hu32)(GetName(), name, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetHashInt64(Resource *resource, const std::string &name, int64_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hi64)
		(static_cast<T*>(resource)->*set_u.hi64)(GetName(), name, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetHashUInt64(Resource *resource, const std::string &name, uint64_t value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hu64)
		(static_cast<T*>(resource)->*set_u.hu64)(GetName(), name, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetHashDouble(Resource *resource, const std::string &name, double value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hd)
		(static_cast<T*>(resource)->*set_u.hd)(GetName(), name, value);
}

template <class T>
void ResourceAttrInfoP<T>::SetHashString(Resource *resource, const std::string &name, const std::string &value) {
	assert(resource->GetTypeId() == GetTypeId());
	if (set_u.hs)
		(static_cast<T*>(resource)->*set_u.hs)(GetName(), name, value);
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
	assert(type == HASH_INT32 || type == HASH_UINT32 || HASH_INT64 || HASH_UINT64 || type == HASH_STRING || type == HASH_IP);
	if (get_hash_keys)
		return (static_cast<T*>(resource)->*get_hash_keys)(GetName());
	return new std::vector<std::string>();
}

template <class T>
std::vector<int32_t> *ResourceAttrInfoP<T>::GetValuesInt32(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_INT32 || type == ARRAY_INT32);
	if (get_all_values_u.i32)
		return (static_cast<T*>(resource)->*get_all_values_u.i32)(GetName());
	return new std::vector<int32_t>();
}

template <class T>
std::vector<uint32_t> *ResourceAttrInfoP<T>::GetValuesUInt32(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_UINT32 || type == ARRAY_UINT32);
	if (get_all_values_u.u32)
		return (static_cast<T*>(resource)->*get_all_values_u.u32)(GetName());
	return new std::vector<uint32_t>();
}

template <class T>
std::vector<int64_t> *ResourceAttrInfoP<T>::GetValuesInt64(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_INT64 || type == ARRAY_INT64);
	if (get_all_values_u.i64)
		return (static_cast<T*>(resource)->*get_all_values_u.i64)(GetName());
	return new std::vector<int64_t>();
}

template <class T>
std::vector<uint64_t> *ResourceAttrInfoP<T>::GetValuesUInt64(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_UINT64 || type == ARRAY_UINT64);
	if (get_all_values_u.u64)
		return (static_cast<T*>(resource)->*get_all_values_u.u64)(GetName());
	return new std::vector<uint64_t>();
}

template <class T>
std::vector<double> *ResourceAttrInfoP<T>::GetValuesDouble(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_UINT64 || type == ARRAY_UINT64);
	if (get_all_values_u.d)
		return (static_cast<T*>(resource)->*get_all_values_u.d)(GetName());
	return new std::vector<double>();
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
std::vector<IpAddr> *ResourceAttrInfoP<T>::GetValuesIpAddr(Resource *resource) {
	assert(resource->GetTypeId() == GetTypeId());
	assert(type == HASH_IP || type == ARRAY_IP);
	if (get_all_values_u.ip)
		return (static_cast<T*>(resource)->*get_all_values_u.ip)(GetName());
	return new std::vector<IpAddr>();
}

template <class T>
void ResourceAttrInfoP<T>::InitInt32(const char *name, int32_t (T::*get)(const std::string&), void (T::*set)(const std::string&, int32_t)) {
	type = INT32;
	this->name = name;
	get_u.i32 = get;
	set_u.i32 = set;
}

template <class T>
void ResourceAttrInfoP<T>::InitUInt32(const char *name, uint32_t (T::*get)(const std::string&), void (T::*set)(const std::string&, uint32_t)) {
	type = UINT32;
	this->name = name;
	get_u.u32 = get;
	set_u.u32 = set;
}

template <class T>
void ResourceAttrInfoP<T>::InitInt64(const char *name, int64_t (T::*get)(const std::string&), void (T::*set)(const std::string&, int64_t)) {
	type = INT64;
	this->name = name;
	get_u.i64 = get;
	set_u.i64 = set;
}

template <class T>
void ResourceAttrInfoP<T>::InitUInt64(const char *name, uint64_t (T::*get)(const std::string&), void (T::*set)(const std::string&, uint64_t)) {
	type = UINT64;
	this->name = name;
	get_u.u64 = get;
	set_u.u64 = set;
}

template <class T>
void ResourceAttrInfoP<T>::InitDouble(const char *name, double (T::*get)(const std::string&), void (T::*set)(const std::string&, double)) {
	type = UINT64;
	this->name = name;
	get_u.d = get;
	set_u.d = set;
}

template <class T>
void ResourceAttrInfoP<T>::InitString(const char *name, std::string (T::*get)(const std::string&), void (T::*set)(const std::string&, const std::string&)) {
	type = STRING;
	this->name = name;
	get_u.s = get;
	set_u.s = set;
}

template <class T>
void ResourceAttrInfoP<T>::InitIpAddr(const char *name, IpAddr (T::*get)(const std::string&), void (T::*set)(const std::string&, IpAddr&)) {
	type = IP;
	this->name = name;
	get_u.ip = get;
	set_u.ip = set;
}

template <class T>
void ResourceAttrInfoP<T>::InitArrayInt32(const char *name, int32_t (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, int32_t), void (T::*clear)(const std::string&), int (T::*count)(const std::string&)) {
	type = ARRAY_INT32;
	this->name = name;
	get_u.ai32 = get;
	set_u.ai32 = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoP<T>::InitArrayUInt32(const char *name, uint32_t (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, uint32_t), void (T::*clear)(const std::string&), int (T::*count)(const std::string&)) {
	type = ARRAY_UINT32;
	this->name = name;
	get_u.au32 = get;
	set_u.au32 = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoP<T>::InitArrayInt64(const char *name, int64_t (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, int64_t), void (T::*clear)(const std::string&), int (T::*count)(const std::string&)) {
	type = ARRAY_INT64;
	this->name = name;
	get_u.ai64 = get;
	set_u.ai64 = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoP<T>::InitArrayUInt64(const char *name, uint64_t (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, uint64_t), void (T::*clear)(const std::string&), int (T::*count)(const std::string&)) {
	type = ARRAY_UINT64;
	this->name = name;
	get_u.au64 = get;
	set_u.au64 = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoP<T>::InitArrayDouble(const char *name, double (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, double), void (T::*clear)(const std::string&), int (T::*count)(const std::string&)) {
	type = ARRAY_UINT64;
	this->name = name;
	get_u.ad = get;
	set_u.ad = set;
	clear_all = clear;
	this->count = count;
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
void ResourceAttrInfoP<T>::InitArrayIpAddr(const char *name, IpAddr (T::*get)(const std::string&, int), void (T::*set)(const std::string&, int, IpAddr&), void (T::*clear)(const std::string&), int (T::*count)(const std::string&)) {
	type = ARRAY_IP;
	this->name = name;
	get_u.aip = get;
	set_u.aip = set;
	clear_all = clear;
	this->count = count;
}

template <class T>
void ResourceAttrInfoP<T>::InitHashInt32(const char *name, int32_t (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, int32_t), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<int32_t> *(T::*values)(const std::string&)) {
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
void ResourceAttrInfoP<T>::InitHashUInt32(const char *name, uint32_t (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, uint32_t), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<uint32_t> *(T::*values)(const std::string&)) {
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
void ResourceAttrInfoP<T>::InitHashInt64(const char *name, int64_t (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, int64_t), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<int64_t> *(T::*values)(const std::string&)) {
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
void ResourceAttrInfoP<T>::InitHashUInt64(const char *name, uint64_t (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, uint64_t), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<uint64_t> *(T::*values)(const std::string&)) {
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
void ResourceAttrInfoP<T>::InitHashDouble(const char *name, double (T::*get)(const std::string&, const std::string&), void (T::*set)(const std::string&, const std::string&, double), void (T::*clear)(const std::string&), void (T::*deleteItem)(const std::string&, const std::string&), int (T::*count)(const std::string&), std::vector<std::string> *(T::*keys)(const std::string&), std::vector<double> *(T::*values)(const std::string&)) {
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
