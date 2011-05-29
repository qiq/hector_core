/**
 *
 */

#include <assert.h>
#include "TestServer.h"
#include "ProcessingEngine.h"

using namespace std;

log4cxx::LoggerPtr TestServer::logger(log4cxx::Logger::getLogger("servers.TestServer"));

TestServer::TestServer(ObjectRegistry *objects, vector<ProcessingEngine*> *engines): BaseServer(objects, engines) {
}

bool TestServer::Init(std::vector<std::pair<std::string, std::string> > *params) {
	return BaseServer::Init(params);
}

bool TestServer::HandleExtension(SimpleHTTPConn *conn) {
	return false;
}

// factory functions

extern "C" BaseServer* hector_server_create(ObjectRegistry *objects, vector<ProcessingEngine*> *engines) {
	return new TestServer(objects, engines);
}
