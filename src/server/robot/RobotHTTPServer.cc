/**
 *
 */

#include "RobotHTTPServer.h"

RobotHTTPServer::RobotHTTPServer(Server *server) {
	this->server = server;
}

bool RobotHTTPServer::HandleRequest(SimpleHTTPConn *conn) {
	string method = conn->getRequestMethod();
	if (method == "SHUTDOWN") {
		conn->setResponseCode(200, "OK");
		conn->appendResponseBody("Shutting down\r\n");
		conn->appendResponseBody(conn->getRequestArgs().c_str());
		conn->appendResponseBody("\r\n");

		this->setRunning(false);
		return true;
	}
	return false;
}

// factory functions

extern "C" SimpleHTTPServer* create(Server *server) {
	return new RobotHTTPServer(server);
}

extern "C" void destroy(SimpleHTTPServer* p) {
	delete p;
}
