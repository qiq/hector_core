/**
 * Simple multi-threaded HTTP server. Descendant must define HandleRequest method.
 */

#ifndef _LIB_SERVER_SIMPLE_HTTP_SERVER_
#define _LIB_SERVER_SIMPLE_HTTP_SERVER_

#include <config.h>

#include "SimpleHTTPConn.h"
#include "SimpleServer.h"

class Config;

class SimpleHTTPServer : public SimpleServer {
public:
	SimpleHTTPServer() {};
	~SimpleHTTPServer() {};
	virtual bool Init(std::vector<std::pair<std::string, std::string> > *params) = 0;
	virtual bool HandleRequest(SimpleHTTPConn *conn) = 0;

	void Request(int fd);
};

#endif
