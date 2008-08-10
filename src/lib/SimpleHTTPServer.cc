/**
 * Simple multi-threaded server
 */

#include <pthread.h>
#include "SimpleHTTPServer.h"
#include "SimpleHTTPConn.h"

void SimpleHTTPServer::ServiceThread(int fd) {
	pthread_detach(pthread_self());
	SimpleHTTPConn *conn = new SimpleHTTPConn(fd);
	while (1) {
		bool result = conn->readRequest();
		// timeout or error
		if (!result)
			break;
		if (!HandleRequest(conn)) {
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
	decreaseThreadCount();
	pthread_exit(0);
}
