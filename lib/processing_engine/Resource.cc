// 
#include <config.h>

#include <log4cxx/level.h>
#include "Resource.h"

using namespace std;

static std::string na = "N/A";

const string &ResourceFieldInfo::getString(Resource *resource) {
	return na;
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

const string &ResourceFieldInfo::getString2(Resource *resource, const string &name) {
	return na;
}

const string &ResourceFieldInfo::getStringN(Resource *resource, int index) {
	return na;
}

void ResourceFieldInfo::setString(Resource *resource, const string &name) {
}

void ResourceFieldInfo::setInt(Resource *resource, int value) {
}

void ResourceFieldInfo::setLong(Resource *resource, long value) {
}

void ResourceFieldInfo::setIp4Addr(Resource *resource, ip4_addr_t value) {
}

void ResourceFieldInfo::setIp6Addr(Resource *resource, ip6_addr_t value) {
}

void ResourceFieldInfo::setString2(Resource *resource, const string &name, const string &value) {
}

void ResourceFieldInfo::setStringN(Resource *resource, int index, const string &value) {
}

void ResourceFieldInfo::clear(Resource *resource) {
}

void ResourceFieldInfo::clearString2(Resource *resource, const string &name) {
}

void ResourceFieldInfo::clearStringN(Resource *resource, int index) {
}

