/**
 * Common functions, macros, etc
 */

#ifndef _LIB_COMMON_H_
#define _LIB_COMMON_H_

#include <config.h>

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/time.h>
#include <vector>
#include <tr1/unordered_map>
#include <log4cxx/logger.h>

static std::string empty_string;

inline void die(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

std::string &skipWs(std::string &data);
std::string &chomp(std::string &data);
void splitOnWs(std::vector<std::string> &result, std::string &data);
void split(std::vector<std::string> &result, std::string &data, char split);
std::string join(std::string &glue, std::vector<std::string> &data);

inline std::string &replace_all(std::string &s, const std::string &sub, const std::string &other) {
	size_t offset = 0;
	while (true) {
		offset = s.find(sub, offset);
		if (offset == s.npos)
			break;
		s.replace(offset, sub.size(), other);
		offset += other.size();
	}
	return s;
}

inline uint32_t NativeEndian(uint32_t le) {
	// Big-endian: convert from little endian (default) to native Big-endian
	return htons(0xabcd) == 0xabcd ? htonl(le) : le;
}

/*void to_lowercase(string &s) {
	for (string::iterator i = s.begin(); i != s.end(); i++)
		*i = tolower(*i);
}*/

/*struct string_hash : public unary_function<string, size_t> {
	size_t operator() (const string &v) const {
		return std::tr1::hash<const char*>()(v.c_str());
	}
};*/

int WriteBytes(int fd, const char *data, int size);
int ReadBytes(int fd, char *data, int size);

//uint32_t bytes2int(char (*bytes)[4]);
//void int2bytes(uint32_t n, char (*bytes)[4]);

int str2bool(const char *value);
char *bool2str(bool value);
int str2int(const char *value);
char *int2str(int value);
long str2long(const char *value);
char *long2str(long value);

struct timeval ConstructTimeout(int usec);

#endif
