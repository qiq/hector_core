/**
 *
 */

#include "RobotHTTPServer.h"

bool RobotHTTPServer::HandleRequest(SimpleHTTPConn *conn) {
	string method = conn->getRequestMethod();
	if (method == "SHUTDOWN") {
		conn->setResponseCode(200, "OK");
		conn->appendResponseBody("Shutting down\r\n");
		conn->appendResponseBody(conn->getRequestArgs().c_str());
		conn->appendResponseBody("\r\n");
		return true;
	}
	return false;
}
