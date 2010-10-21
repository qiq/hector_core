// 
#include <config.h>

#include <log4cxx/level.h>
#include "Resource.h"

using namespace std;

const char *ResourceInfo::getString(Resource *resource) {
	return NULL;
}

int ResourceInfo::getInt(Resource *resource) {
	return -1;
}

long ResourceInfo::getLong(Resource *resource) {
	return -1;
}

ip4_addr_t ResourceInfo::getIp4Addr(Resource *resource) {
	return ip4_addr_empty;
}

ip6_addr_t ResourceInfo::getIp6Addr(Resource *resource) {
	return ip6_addr_empty;
}

const char *ResourceInfo::getString2(Resource *resource, const char *name) {
	return NULL;
}

const char *ResourceInfo::getStringN(Resource *resource, int index) {
	return NULL;
}

void ResourceInfo::setString(Resource *resource, const char *name) {
}

void ResourceInfo::setInt(Resource *resource, int value) {
}

void ResourceInfo::setLong(Resource *resource, long value) {
}

void ResourceInfo::setIp4Addr(Resource *resource, ip4_addr_t value) {
}

void ResourceInfo::setIp6Addr(Resource *resource, ip6_addr_t value) {
}

void ResourceInfo::setString2(Resource *resource, const char *name, const char *value) {
}

void ResourceInfo::setStringN(Resource *resource, int index, const char *value) {
}

void ResourceInfo::clear(Resource *resource) {
}

void ResourceInfo::clearString2(Resource *resource, const char *name) {
}

void ResourceInfo::clearStringN(Resource *resource, int index) {
}

