#ifndef _EXTERNAL_PROCESS_H_
#define _EXTERNAL_PROCESS_H_

#include <log4cxx/logger.h>
#include <unistd.h>

class ExternalProcess {
	pid_t pid;

	int fdin;
	int fdout;
	int fderr;

	char stderr_buffer[1024];
	int stderr_buffer_len;

        static log4cxx::LoggerPtr logger;
public:
	ExternalProcess();
	~ExternalProcess();

	bool Init(const char *path, const char *argv[]);
	int readWrite(const char *writeBuffer, int writeBufferLen, char *readBuffer, int readBufferLen, bool waitForRead = true);
};

#endif
