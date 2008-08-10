/**
 * Common functions, macros, etc
 */

#include "common.h"
#include <unistd.h>

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

/*void log_config_error(log4cxx::LoggerPtr logger, int line, int column, const char *msg, ...) {
	char s[1024];
	va_list args;
	va_start(args, msg);
	vsnprintf(s, sizeof(s)-30, msg, args);
	va_end(args);
	snprintf(s+strlen(s), 30, " [%d, %d]", line, column);
	LOG4CXX_ERROR(logger, s);
}*/
