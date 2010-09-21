#ifndef _LIB_PROCESS_CONNECTION_H_
#define _LIB_PROCESS_CONNECTION_H_

#include <log4cxx/logger.h>
#include <unistd.h>
#include "Connection.h"

class ProcessConnection : public Connection {
	pid_t pid;

	int fdin;
	int fdout;
	int fderr;

	char stderr_buffer[1024];
	int stderr_buffer_len;

        static log4cxx::LoggerPtr logger;
public:
	ProcessConnection();
	~ProcessConnection();

	bool Init(const char *path, const char *argv[], const char *env[]);
	int ReadWrite(const char *writeBuffer, int writeBufferLen, char *readBuffer, int readBufferLen, bool waitForRead = true);
};

#endif
