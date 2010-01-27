#ifndef _STDIO_CONNECTION_H_
#define _STDIO_CONNECTION_H_

#include <log4cxx/logger.h>
#include "Connection.h"

class StdioConnection : public Connection {
        static log4cxx::LoggerPtr logger;
public:
	StdioConnection();
	~StdioConnection();

	int ReadWrite(const char *writeBuffer, int writeBufferLen, char *readBuffer, int readBufferLen, bool waitForRead = true);
};

#endif
