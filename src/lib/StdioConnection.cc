#include <errno.h>
#include <string.h>	// for strerror()
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include "StdioConnection.h"

log4cxx::LoggerPtr StdioConnection::logger(log4cxx::Logger::getLogger("lib.StdioConnection"));

StdioConnection::StdioConnection() {
}

StdioConnection::~StdioConnection() {
}

int StdioConnection::ReadWrite(const char *writeBuffer, int writeBufferLen, char *readBuffer, int readBufferLen, bool waitForRead) {
	bool write_done = (writeBuffer == NULL || writeBufferLen == 0) ? true : false;
	bool read_done = (readBuffer == NULL || readBufferLen == 0) ? true : false;
	int read_bytes = 0;
	int written_bytes = 0;
	bool timeout = false;

	while (!write_done || (waitForRead && !read_done) || !timeout) {
		fd_set read_fd;
		fd_set write_fd;
		FD_ZERO(&read_fd);
		FD_ZERO(&write_fd);
		int fdmax = 0;
		if (!write_done) {
			FD_SET(STDOUT_FILENO, &write_fd);
			if (STDOUT_FILENO > fdmax)
				fdmax = STDOUT_FILENO;
		}
		if (!read_done) {
			FD_SET(STDIN_FILENO, &read_fd);
			if (STDIN_FILENO > fdmax)
				fdmax = STDIN_FILENO;
		}
		struct timeval select_timeout = { 0, 0 };
		int ready = select(fdmax+1, &read_fd, write_done ? NULL : &write_fd, NULL, read_done && write_done ? &select_timeout : NULL);
		if (ready < 0) {
			LOG4CXX_ERROR(logger, "Error selecting: " << strerror(errno));
			return -1;
		}
		// timeout
		timeout = ready == 0 ? true : false;
		if (timeout)
			continue;
		// ready to write to stdin
		if (!write_done && FD_ISSET(STDOUT_FILENO, &write_fd)) {
			int w = write(STDOUT_FILENO, writeBuffer + written_bytes, writeBufferLen - written_bytes);
			if (w < 0) {
				LOG4CXX_INFO(logger, strerror(errno));
				write_done = true;
			} else if (w > 0) {
				written_bytes += w;
				if (written_bytes == writeBufferLen)
					write_done = true;
			}
		}
		// ready to read stdout
		if (!read_done && FD_ISSET(STDIN_FILENO, &read_fd)) {
			int r = read(STDIN_FILENO, readBuffer + read_bytes, readBufferLen - read_bytes);
			if (r < 0) {
				LOG4CXX_INFO(logger, strerror(errno));
				read_done = true;
			} else if (r > 0) {
				read_bytes += r;
				if (read_bytes == readBufferLen)
					read_done = true;
			}
		}
	}
	return read_bytes + written_bytes;
}
