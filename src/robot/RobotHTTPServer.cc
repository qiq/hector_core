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
	pthread_t thread;
	pthread_create(&thread, NULL, run_thread, (void *)this);
}

void RobotHTTPServer::ProcessGET(SimpleHTTPConn *conn) {
fprintf(stderr, "GET!\n");
	conn->setResponseCode(200, "xOK");
	conn->appendResponseHeader("blah: blah");
	conn->appendResponseBody("hehe!");
}

void RobotHTTPServer::ProcessPOST(SimpleHTTPConn *conn) {
}
