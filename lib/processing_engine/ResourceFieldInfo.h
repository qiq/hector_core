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
		STRING2,	// string->string
		STRINGN,	// [string]
	} FieldType;

	ResourceFieldInfo() {};
	virtual ~ResourceFieldInfo() {};
	FieldType getType();

	virtual const std::string &getString(Resource*) = 0;
	virtual int getInt(Resource*) = 0;
	virtual long getLong(Resource*) = 0;
	virtual IpAddr &getIpAddr(Resource*) = 0;
	virtual const std::string &getString2(Resource*, const std::string&) = 0;
//	virtual const std::string &getStringN(Resource*, int) = 0;

	virtual void setString(Resource*, const std::string&) = 0;
	virtual void setInt(Resource*, int) = 0;
	virtual void setLong(Resource*, long) = 0;
	virtual void setIpAddr(Resource*, IpAddr&) = 0;
	virtual void setString2(Resource*, const std::string&, const std::string&) = 0;
//	virtual void setStringN(Resource*, int, const std::string&) = 0;

	virtual void clear(Resource*) = 0;
	virtual void clearString2(Resource*, const std::string&) = 0;
//	virtual void clearStringN(Resource*, int) = 0;

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
	const std::string &getString2(Resource*, const std::string&);
//	const std::string &getStringN(Resource*, int);

	void setString(Resource*, const std::string&);
	void setInt(Resource*, int);
	void setLong(Resource*, long);
	void setIpAddr(Resource*, IpAddr&);
	void setString2(Resource*, const std::string&, const std::string&);
//	void setStringN(Resource*, int, const std::string&);

	void clear(Resource*);
	void clearString2(Resource*, const std::string&);
//	void clearStringN(Resource*, int);

protected:
	union {
		const std::string &(T::*s)();
		int (T::*i)();
		long (T::*l)();
		IpAddr &(T::*ip)();
		const std::string &(T::*s2)(const std::string&);
		const std::string &(T::*sn)(int);
	} get_u;
	union {
		void (T::*s)(const std::string&);
		void (T::*i)(int);
		void (T::*l)(long);
		void (T::*ip)(IpAddr&);
		void (T::*s2)(const std::string&, const std::string&);
		void (T::*sn)(int, const std::string&);
	} set_u;
	union {
		void (T::*c)();
		void (T::*s2)(const std::string&);
		void (T::*sn)(int);
	} clear_u;
};

ResourceFieldInfo::FieldType ResourceFieldInfo::getType() {
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
	return get_u.ip ? (static_cast<T*>(resource)->*get_u.ip)() : IpAddr::ipAddrEmpty;
}

template <class T>
const std::string &ResourceFieldInfoT<T>::getString2(Resource *resource, const std::string &name) {
	assert(resource->getTypeId() == T::typeId);
	return get_u.s2 ? (static_cast<T*>(resource)->*get_u.s2)(name) : empty_string;
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
void ResourceFieldInfoT<T>::setString2(Resource *resource, const std::string &name, const std::string &value) {
	assert(resource->getTypeId() == T::typeId);
	if (set_u.s2)
		(static_cast<T*>(resource)->*set_u.s2)(name, value);
}

template <class T>
void ResourceFieldInfoT<T>::clear(Resource *resource) {
	assert(resource->getTypeId() == T::typeId);
	if (clear_u.c)
		(static_cast<T*>(resource)->*clear_u.c)();
}

template <class T>
void ResourceFieldInfoT<T>::clearString2(Resource *resource, const std::string &name) {
	assert(resource->getTypeId() == T::typeId);
	if (clear_u.s2)
		(static_cast<T*>(resource)->*clear_u.s2)(name);
}

#endif
