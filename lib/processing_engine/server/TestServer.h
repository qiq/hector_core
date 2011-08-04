/*
 *
 */

#ifndef _LIB_PROCESSING_ENGINE_TEST_SERVER_
#define _LIB_PROCESSING_ENGINE_TEST_SERVER_

#include <config.h>

#include <vector>
#include <tr1/unordered_map>
#include "BaseServer.h"

class TestServer : public BaseServer {
public:
	TestServer(ObjectRegistry *objects, vector<ProcessingEngine*> *engines);
	~TestServer() {};
	bool Init(std::vector<std::pair<std::string, std::string> > *params);
	bool HandleExtension(SimpleHTTPConn *conn);

protected:
	static log4cxx::LoggerPtr logger;
};

#endif
