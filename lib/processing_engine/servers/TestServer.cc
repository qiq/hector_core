/**
 *
 */

#include "TestServer.h"
#include "Object.h"

log4cxx::LoggerPtr TestServer::logger(log4cxx::Logger::getLogger("servers.robot.TestServer"));

TestServer::TestServer(ObjectRegistry *objects) {
	this->objects = objects;
}

bool TestServer::HandleRequest(SimpleHTTPConn *conn) {
	string method = conn->getRequestMethod();
	string args = conn->getRequestArgs();
	if (args.substr(0, 1) == "/")
		args = args.substr(1);
	if (method == "GET") {
		LOG4CXX_INFO(logger, "GET " << args);
		if (!args.empty()) {
			size_t dot = args.find_first_of('.');
			if (dot != string::npos) {
				// get value
				string object = args.substr(0, dot);
				string property = args.substr(dot+1);
				char *value = objects->getObjectValue(object.c_str(), property.c_str());
				if (value) {
					conn->setResponseCode(200, "OK");
					conn->appendResponseBody(value);
				} else {
					conn->ErrorResponse(400, "Object/property not found", "");
				}
				free(value);
			} else {
				// list object properties
				Object *object = objects->getObject(args.c_str());
				if (object) {
					vector<string> *names = object->listNames();
					for (vector<string>::iterator iter = names->begin(); iter != names->end(); ++iter) {
						conn->appendResponseBody(iter->c_str());
						conn->appendResponseBody("\r\n");
					}
					delete names;
				} else {
					// object not found
					conn->ErrorResponse(400, "Object not found", "");
				}
			}
		} else {
			// list objects
			vector<string> *all = objects->getIds();
			conn->setResponseCode(200, "OK");
			for (vector<string>::iterator iter = all->begin(); iter != all->end(); ++iter) {
				conn->appendResponseBody(iter->c_str());
				conn->appendResponseBody("\r\n");
			}
			delete all;
		}
		return true;
	} else if (method == "SET") {
		LOG4CXX_INFO(logger, "SET " << args);
		if (!args.empty()) {
			size_t dot = args.find_first_of('.');
			if (dot != string::npos) {
				string object = args.substr(0, dot);
				string property = args.substr(dot+1);
				string body = conn->getRequestBody();
				size_t eoln = args.find_first_of("\r\n");
				string value = eoln != string::npos ? body.substr(0, eoln): body;
				if (objects->setObjectValue(object.c_str(), property.c_str(), value.c_str())) {
					conn->setResponseCode(200, "OK");
				} else {
					conn->ErrorResponse(400, "Object/property not found", "");
				}
			}
		} else {
			// error: no object.property given
			conn->ErrorResponse(400, "No object.property argument given", "");
		}
		return true;
	} else if (method == "SHUTDOWN") {
		LOG4CXX_INFO(logger, "SHUTDOWN");
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

extern "C" SimpleHTTPServer* create(ObjectRegistry *objects) {
	return new TestServer(objects);
}
