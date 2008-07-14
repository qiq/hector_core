/**
 *
 */

#include "RobotHTTPServer.h"

void *run_thread(void *ptr) {
	RobotHTTPServer *server = (RobotHTTPServer*)ptr;
	server->runThread();
	return NULL;
}

void RobotHTTPServer::runThread() {
	pthread_detach(pthread_self());
	server->Server();
}

RobotHTTPServer::RobotHTTPServer() {
	server = NULL;
}

RobotHTTPServer::~RobotHTTPServer() {
	if (server)
		server->Shutdown();
}

void RobotHTTPServer::Run() {
	server = new SimpleHTTPServer(this, NULL, 1234);
	server->setMaxThreads(1);
	pthread_t thread;
	pthread_create(&thread, NULL, run_thread, (void *)this);
}

bool RobotHTTPServer::HandleRequest(SimpleHTTPConn *conn) {
	string method = conn->getRequestMethod();
	if (method == "SHUTDOWN") {
		conn->setResponseCode(200, "OK");
		conn->appendResponseBody("Shutting down\r\n");
		return true;
	}
	return false;
}
