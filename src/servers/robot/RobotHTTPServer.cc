/**
 *
 */

#include "RobotHTTPServer.h"
#include "Object.h"

RobotHTTPServer::RobotHTTPServer(ObjectRegistry *objects) {
	this->objects = objects;
}

bool RobotHTTPServer::HandleRequest(SimpleHTTPConn *conn) {
	string method = conn->getRequestMethod();
	if (method == "SHUTDOWN") {
		conn->setResponseCode(200, "OK");
		conn->appendResponseBody("Shutting down\r\n");
conn->appendResponseBody(objects->getObjectValue("save-resource-module", ""));
conn->appendResponseBody("\r\n");
		conn->appendResponseBody(conn->getRequestArgs().c_str());
		conn->appendResponseBody("\r\n");

		this->setRunning(false);
		return true;
	}
	return false;
}

// factory functions

extern "C" SimpleHTTPServer* create(ObjectRegistry *objects) {
	return new RobotHTTPServer(objects);
}

extern "C" void destroy(SimpleHTTPServer* p) {
	delete p;
}
