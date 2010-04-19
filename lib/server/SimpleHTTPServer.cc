/**
 * Simple multi-threaded server
 */

#include <config.h>

#include "SimpleHTTPConn.h"
#include "SimpleHTTPServer.h"

void SimpleHTTPServer::Request(int fd) {
	SimpleHTTPConn *conn = new SimpleHTTPConn(fd);
	while (1) {
		bool result;
		result = conn->ReadRequest();
		// timeout or error
		if (!result)
			break;
		if (!HandleRequest(conn)) {
			char s[1000];
			snprintf(s, sizeof(s), "Method %s not implemented", conn->getRequestMethod().c_str());
			conn->ErrorResponse(501, "Not implemented", s);
		}

		conn->SendResponse();
		if (!conn->isKeepAlive())
			break; 
		conn->Clear();
	}
	delete conn;
}
