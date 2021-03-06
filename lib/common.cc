/**
 * Common functions, macros, etc
 */

#include <config.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ltdl.h>
#include "common.h"

using namespace std;

string &skipWs(string &data) {
	size_t offset = data.find_first_not_of(" \t\n\r");
	if (offset != string::npos)
		data.erase(0, offset);
	else
		data.clear();
	return data;
}

string &chomp(string &data) {
	size_t offset = data.find_last_not_of(" \t\n\r");
	if (offset != string::npos)
		data.erase(offset+1);
	else
		data.clear();
	return data;
}

void splitOnWs(vector<string> &result, string &data) {
	result.clear();
	unsigned b = 0;
	unsigned e;
	while (b < data.length()) {
		while (b < data.length() && isspace(data[b]))
			b++;
		e = b;
		while (e < data.length() && !isspace(data[e]))
			e++;
		if (b < data.length()) {
			result.push_back(data.substr(b, e-b));
			b = e;
		}
	}
}

void split(vector<string> &result, char split, string &data) {
	result.clear();
	unsigned b = 0;
	for (unsigned i = 0; i < data.length(); i++) {
		if (data[i] == split) {
			result.push_back(data.substr(b, i-b));
			b = i+1;
		}
	}
	if (b < data.length())
		result.push_back(data.substr(b));
}

string join(string &glue, vector<string> &data) {
	string result;
	bool first = true;
	for (vector<string>::iterator iter = data.begin(); iter != data.end(); ++iter) {
		if (first)
			first = false;
		else
			result.append(glue);
		result.append(*iter);
	}
	return result;
}

string join(char glue, vector<string> &data) {
	string result;
	bool first = true;
	for (vector<string>::iterator iter = data.begin(); iter != data.end(); ++iter) {
		if (first)
			first = false;
		else
			result.append(1, glue);
		result.append(*iter);
	}
	return result;
}

int WriteBytes(int fd, const char *data, int size) {
	int offset = 0;
	while (size > 0) {
		ssize_t wr = write(fd, (void*)(data+offset), size);
		if (wr < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		} else if (wr == 0) {
			return false;
		}
		size -= wr;
		offset += wr;
	}
	return offset;
}

int ReadBytes(int fd, char *data, int size) {
	int offset = 0;
	while (size > 0) {
		ssize_t rd = read(fd, (void*)(data + offset), size);
		if (rd < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		} else if (rd == 0) {
			return offset;
		}
		size -= rd;
		offset += rd;
	}
	return offset;
}

/*
htonl()/ntohl() used instead
uint32_t bytes2int(char (*bytes)[4]) {
	return (*bytes)[3] << 24 | (*bytes[2]) << 16 | (*bytes[1]) << 8 | (*bytes[0]);
}

void int2bytes(uint32_t n, char (*bytes)[4]) {
	(*bytes)[0] = (char)(n & 0xFF);
	(*bytes)[1] = (char)(n >> 8 & 0xFF);
	(*bytes)[2] = (char)(n >> 16 & 0xFF);
	(*bytes)[3] = (char)(n >> 24 & 0xFF);
}
*/

int str2bool(const char *value) {
        if (!strcmp(value, "0") || value[0] == 'f' || value[0] == 'F')
		return 0;
        if (!strcmp(value, "1") || value[0] == 't' || value[0] == 'T')
		return 1;
	return -1;
}

char *bool2str(bool value) {
	return value ? strdup("1") : strdup("0");
}

int str2int(const char *value) {
	return atoi(value);
}

char *int2str(int value) {
	char s[1024];
	snprintf(s, sizeof(s), "%d", value);
	return strdup(s);
}

int64_t str2long(const char *value) {
	int64_t result = 0;
	sscanf(value, "%" PRId64, &result);
	return result;
}

char *long2str(int64_t value) {
	char s[1024];
	snprintf(s, sizeof(s), "%" PRId64, value);
	return strdup(s);
}

double str2double(const char *value) {
	return atof(value);
}

char *double2str(double value) {
	char s[1024];
	snprintf(s, sizeof(s), "%lf", value);
	return strdup(s);
}

struct timeval ConstructTimeout(int usec) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	tv.tv_sec += usec / 1000000;
	tv.tv_usec += usec % 1000000;
	if (tv.tv_usec > 1000000) {
		tv.tv_usec -= 1000000;
		tv.tv_sec++;
	}
	return tv;
}
