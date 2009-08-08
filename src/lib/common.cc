/**
 * Common functions, macros, etc
 */

#include <config.h>

#include <unistd.h>
#include <ltdl.h>
#include "common.h"

int writeBytes(int fd, const char *s, int length) {
	int written = 0;
	while (length - written > 0) {
		int w = write(fd, s + written, length - written);
		if (w < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		written += w;
	}
	return written;
}

int readBytes(int fd, char *s, int length) {
	int rd = 0;
	while (length - rd > 0) {
		int r = read(fd, (void*)(s + rd), length - rd);
		if (r < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		if (r == 0)
			return rd;
		rd += r;
	}
	return rd;
}

uint32_t bytes2int(char (*bytes)[4]) {
	return (*bytes)[3] << 24 | (*bytes[2]) << 16 | (*bytes[1]) << 8 | (*bytes[0]);
}

void int2bytes(uint32_t n, char (*bytes)[4]) {
	(*bytes)[0] = (char)(n & 0xFF);
	(*bytes)[1] = (char)(n >> 8 & 0xFF);
	(*bytes)[2] = (char)(n >> 16 & 0xFF);
	(*bytes)[3] = (char)(n >> 24 & 0xFF);
}

/*void log_config_error(log4cxx::LoggerPtr logger, int line, int column, const char *msg, ...) {
	char s[1024];
	va_list args;
	va_start(args, msg);
	vsnprintf(s, sizeof(s)-30, msg, args);
	va_end(args);
	snprintf(s+strlen(s), 30, " [%d, %d]", line, column);
	LOG4CXX_ERROR(logger, s);
}*/

// TODO: fix memory leaks
void *loadLibrary(const char *lib, const char *sym) {
	if (lt_dlinit() != 0)
		return NULL;
	lt_dlhandle handle = lt_dlopen(lib);
	if (handle == NULL) {
fprintf(stderr, lt_dlerror());
		return NULL;
}
	void *p = lt_dlsym(handle, sym);
if (!p)
fprintf(stderr, lt_dlerror());
	return p;
}

