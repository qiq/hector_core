/**
 * Simple multi-threaded server
 */

#include <config.h>

#include "SimpleHTTPConn.h"
#include "SimpleHTTPServer.h"

void SimpleHTTPServer::request(int fd) {
	SimpleHTTPConn *conn = new SimpleHTTPConn(fd);
	while (1) {
		bool result;
		result = conn->readRequest();
		// timeout or error
		if (!result)
			break;
		if (!handleRequest(conn)) {
			char s[1000];
			snprintf(s, sizeof(s), "Method %s not implemented", conn->getRequestMethod().c_str());
			conn->errorResponse(501, "Not implemented", s);
		}

		conn->sendResponse();
		if (!conn->isKeepAlive())
			break; 
		conn->clear();
	}
	delete conn;
}
