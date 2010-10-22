// 
#include <config.h>

#include <log4cxx/level.h>
#include "Resource.h"

using namespace std;

const char *ResourceFieldInfo::getString(Resource *resource) {
	return NULL;
}

int ResourceFieldInfo::getInt(Resource *resource) {
	return -1;
}

long ResourceFieldInfo::getLong(Resource *resource) {
	return -1;
}

ip4_addr_t ResourceFieldInfo::getIp4Addr(Resource *resource) {
	return ip4_addr_empty;
}

ip6_addr_t ResourceFieldInfo::getIp6Addr(Resource *resource) {
	return ip6_addr_empty;
}

const char *ResourceFieldInfo::getString2(Resource *resource, const char *name) {
	return NULL;
}

const char *ResourceFieldInfo::getStringN(Resource *resource, int index) {
	return NULL;
}

void ResourceFieldInfo::setString(Resource *resource, const char *name) {
}

void ResourceFieldInfo::setInt(Resource *resource, int value) {
}

void ResourceFieldInfo::setLong(Resource *resource, long value) {
}

void ResourceFieldInfo::setIp4Addr(Resource *resource, ip4_addr_t value) {
}

void ResourceFieldInfo::setIp6Addr(Resource *resource, ip6_addr_t value) {
}

void ResourceFieldInfo::setString2(Resource *resource, const char *name, const char *value) {
}

void ResourceFieldInfo::setStringN(Resource *resource, int index, const char *value) {
}

void ResourceFieldInfo::clear(Resource *resource) {
}

void ResourceFieldInfo::clearString2(Resource *resource, const char *name) {
}

void ResourceFieldInfo::clearStringN(Resource *resource, int index) {
}

