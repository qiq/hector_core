/**
 * Simple multi-threaded HTTP server. Descendant must define HandleRequest method.
 */

#ifndef _SIMPLE_HTTP_SERVER_
#define _SIMPLE_HTTP_SERVER_

#include <config.h>

#include "SimpleHTTPConn.h"
#include "SimpleServer.h"

class SimpleHTTPServer : public SimpleServer {
public:
	SimpleHTTPServer() {};
	~SimpleHTTPServer() {};

	virtual bool HandleRequest(SimpleHTTPConn *conn) = 0;

	void Request(int fd);
};

#endif
