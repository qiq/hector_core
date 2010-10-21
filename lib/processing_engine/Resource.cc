// 
#include <config.h>

#include <log4cxx/level.h>
#include "Resource.h"

using namespace std;

const char *getString(Resource *resource) {
	return NULL;
}

int getInt(Resource *resource) {
	return -1;
}

long getLong(Resource *resource) {
	return -1;
}

ip4_addr_t getIp4Addr(Resource *resource) {
	ip4_addr_t addr;
	addr.addr = 0;
	return addr;
}

ip6_addr_t getIp6Addr(Resource *resource) {
	ip6_addr_t addr;
	for (int i = 0; i < 16; i++)
		addr.addr[i] = 0;
	return addr;
}

const char *getString2(Resource *resource, const char *name) {
	return NULL;
}

const char *getStringN(Resource *resource, int index) {
	return NULL;
}

void setString(Resource *resource, const char *name) {
}

void setInt(Resource *resource, int value) {
}

void setLong(Resource *resource, long value) {
}

void setIp4Addr(Resource *resource, ip4_addr_t value) {
}

void setIp6Addr(Resource *resource, ip6_addr_t value) {
}

void setString2(Resource *resource, const char *name, const char *value) {
}

void setStringN(Resource *resource, int index, const char *value) {
}

void clear(Resource *resource) {
}

void clearString2(Resource *resource, const char *name) {
}

void clearStringN(Resource *resource, int index) {
}

