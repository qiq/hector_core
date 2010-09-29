/**
 * Common functions, macros, etc
 */

#ifndef _LIB_COMMON_H_
#define _LIB_COMMON_H_

#include <config.h>

#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <tr1/unordered_map>
#include <log4cxx/logger.h>

inline void die(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

typedef struct {
	uint32_t addr;
} ip4_addr_t;

typedef struct {
	uint8_t addr[16];
} ip6_addr_t;

/*void to_lowercase(string &s) {
	for (string::iterator i = s.begin(); i != s.end(); i++)
		*i = tolower(*i);
}*/

/*struct string_hash : public unary_function<string, size_t> {
	size_t operator() (const string &v) const {
		return std::tr1::hash<const char*>()(v.c_str());
	}
};*/

int writeBytes(int fd, const char *s, int length);
int readBytes(int fd, char *s, int length);

/*
#define LOG_CONFIG_ERROR0(logger, line, column, msg) \
{ \
	char s[1024]; \
	snprintf(s, sizeof(s)-30, msg); \
	snprintf(s+strlen(s), 30, " [%d, %d]", line, column); \
	LOG4CXX_ERROR(logger, s); \
}

#define LOG_CONFIG_ERROR1(logger, line, column, msg, a) \
{ \
	char s[1024]; \
	snprintf(s, sizeof(s)-30, msg, a); \
	snprintf(s+strlen(s), 30, " [%d, %d]", line, column); \
	LOG4CXX_ERROR(logger, s); \
}

#define LOG_CONFIG_INFO0(logger, line, column, msg) \
{ \
	char s[1024]; \
	snprintf(s, sizeof(s)-30, msg); \
	snprintf(s+strlen(s), 30, " [%d, %d]", line, column); \
	LOG4CXX_INFO(logger, s); \
}

#define LOG_CONFIG_INFO1(logger, line, column, msg, a) \
{ \
	char s[1024]; \
	snprintf(s, sizeof(s)-30, msg, a); \
	snprintf(s+strlen(s), 30, " [%d, %d]", line, column); \
	LOG4CXX_INFO(logger, s); \
}*/

//uint32_t bytes2int(char (*bytes)[4]);
//void int2bytes(uint32_t n, char (*bytes)[4]);

int str2bool(const char *value);
char *bool2str(bool value);
int str2int(const char *value);
char *int2str(int value);

#endif
