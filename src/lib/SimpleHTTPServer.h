/**
 * Simple multi-threaded server
 */

#ifndef _SIMPLE_HTTP_SERVER_
#define _SIMPLE_HTTP_SERVER_

#include "SimpleServer.h"
#include "SimpleHTTPConn.h"

class SimpleHTTPServer : public SimpleServer {
public:
	SimpleHTTPServer(const char *addr, int port) : SimpleServer(addr, port) {};
	~SimpleHTTPServer();

	virtual bool HandleRequest(SimpleHTTPConn *conn) = 0;

	void ServiceThread(int fd);
};

#endif
