
#include "ProcessConnection.h"
#include <errno.h>
#include <string.h>	// for strerror()
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

log4cxx::LoggerPtr ProcessConnection::logger(log4cxx::Logger::getLogger("lib.ProcessConnection"));

ProcessConnection::ProcessConnection() {
	stderr_buffer_len = 0;
	pid = 0;
	fdin = -1;
	fdout = -1;
	fderr = -1;
}

ProcessConnection::~ProcessConnection() {
	if (fdin != -1)
		close(fdin);
	if (fdout != -1)
		close(fdout);
	if (fderr != -1)
		close(fderr);
	if (pid) {
		ReadWrite(NULL, 0, NULL, 0, false);
		kill(-pid, 9);
	}
}


bool ProcessConnection::Init(const char *path, const char *argv[], const char *envv[]) {
	int pipein[2];
	int pipeout[2];
	int pipeerr[2];

	if (pipe(pipein) < 0) {
		LOG4CXX_ERROR(logger, "Cannot open pipe: " << strerror(errno));
		return false;
	}
	if (pipe(pipeout) < 0) {
		LOG4CXX_ERROR(logger, "Cannot open pipe: " << strerror(errno));
		return false;
	}
	if (pipe(pipeerr) < 0) {
		LOG4CXX_ERROR(logger, "Cannot open pipe: " << strerror(errno));
		return false;
	}

	pid = fork();
	if (pid < 0) {
		LOG4CXX_ERROR(logger, "Cannot fork: " << strerror(errno));
		return false;
	}
	if (pid == 0) {
		// child
		dup2(pipein[0], STDIN_FILENO);
		close(pipein[0]);
		close(pipein[1]);
		dup2(pipeout[1], STDOUT_FILENO);
		close(pipeout[0]);
		close(pipeout[1]);
		dup2(pipeerr[1], STDERR_FILENO);
		close(pipeerr[0]);
		close(pipeerr[1]);

		// set up environment
		if (envv) {
			for (int i = 0; envv[i]; i++) {
				if (putenv(strdup(envv[i]))) {
					LOG4CXX_WARN(logger, "Invalid environment varialble: " << envv[i]);
				}
			}
		}

		setsid();
		if (execvp(path, (char * const*)argv) == -1) {
			LOG4CXX_ERROR(logger, "Cannot exec: " << strerror(errno));
		}
		exit(255);
	}
	// parent
	close(pipein[0]);
	close(pipeout[1]);
	close(pipeerr[1]);
	fdin = pipein[1];
	fdout = pipeout[0];
	fderr = pipeerr[0];

	return true;
}

int ProcessConnection::ReadWrite(const char *writeBuffer, int writeBufferLen, char *readBuffer, int readBufferLen, bool waitForRead) {
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
			FD_SET(fdin, &write_fd);
			if (fdin > fdmax)
				fdmax = fdin;
		}
		if (!read_done) {
			FD_SET(fdout, &read_fd);
			if (fdout > fdmax)
				fdmax = fdout;
		}
		FD_SET(fderr, &read_fd);
		if (fderr > fdmax)
			fdmax = fderr;
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
		if (!write_done && FD_ISSET(fdin, &write_fd)) {
			int w = write(fdin, writeBuffer + written_bytes, writeBufferLen - written_bytes);
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
		if (!read_done && FD_ISSET(fdout, &read_fd)) {
			int r = read(fdout, readBuffer + read_bytes, readBufferLen - read_bytes);
			if (r < 0) {
				LOG4CXX_INFO(logger, strerror(errno));
				read_done = true;
			} else if (r > 0) {
				read_bytes += r;
				if (read_bytes == readBufferLen)
					read_done = true;
			}
		}
		// ready to read stderr
		if (FD_ISSET(fderr, &read_fd)) {
			int r = read(fderr, stderr_buffer + stderr_buffer_len, sizeof(stderr_buffer)-1 - stderr_buffer_len);
			if (r < 0) {
				LOG4CXX_INFO(logger, strerror(errno));
			} else if (r > 0) {
				stderr_buffer_len += r;
				char *cr = (char *)memrchr(stderr_buffer, '\r', stderr_buffer_len);
				char *lf = (char *)memrchr(stderr_buffer, '\n', stderr_buffer_len);
				char *last = cr > lf ? cr : lf;
				// entire buffer is full?
				if (stderr_buffer_len == sizeof(stderr_buffer)-1)
					last = stderr_buffer + sizeof(stderr_buffer)-1;
				if (last != NULL) {
					*last = '\0';
					LOG4CXX_ERROR(logger, stderr_buffer);
					memcpy(stderr_buffer, last+1, stderr_buffer_len-(last+1-stderr_buffer));
					stderr_buffer_len = stderr_buffer_len-(last+1-stderr_buffer);
				}
			}
		}
	}
	return read_bytes + written_bytes;
}
