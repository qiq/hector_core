/**
 * Common functions, macros, etc
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <pthread.h>
#include <string>
#include <ext/hash_set>

using namespace std;
namespace stdext = ::__gnu_cxx;

inline void die(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	exit(1);
}

/*void to_lowercase(string &s) {
	for (string::iterator i = s.begin(); i != s.end(); i++)
		*i = tolower(*i);
}*/

struct string_hash : public unary_function<string, size_t> {
	size_t operator() (const string &v) const {
		return stdext::hash<char*>()(v.c_str());
	}
};

int writeBytes(int fd, const char *s, int length);
int readBytes(int fd, char *s, int length);

#endif
