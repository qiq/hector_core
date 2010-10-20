/**
 *
 */

#include <assert.h>
#include "TestServer.h"
#include "Object.h"
#include "ProcessingEngine.h"

using namespace std;

log4cxx::LoggerPtr TestServer::logger(log4cxx::Logger::getLogger("servers.TestServer"));

TestServer::TestServer(ObjectRegistry *objects, vector<ProcessingEngine*> *engines) {
	this->objects = objects;
	this->engines = engines;
}

bool TestServer::Init(std::vector<std::pair<std::string, std::string> > *params) {
	if (engines->size() == 0) {
		LOG4CXX_ERROR(logger, "No processing engine in a server");
		return false;
	}
	for (vector<ProcessingEngine*>::iterator iter = engines->begin(); iter != engines->end(); ++iter) {
		name2engine[(*iter)->getId()] = *iter;
	}
	this->resourceId = (*engines)[0]->ResourceNameToId("TestResource");
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
				size_t eoln = body.find_first_of("\r\n");
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
	} else if (method == "PROCESS") {
		LOG4CXX_INFO(logger, "PROCESS " << args);
		skipWs(&args);
		chomp(&args);
		tr1::unordered_map<string, ProcessingEngine*>::iterator iter = name2engine.find(args);
		if (iter == name2engine.end()) {
			LOG4CXX_ERROR(logger, "ProcessingEngine " << args << " not found");
			conn->setResponseCode(500, "ProcessorEngine not found");
			return true;
		}
		ProcessingEngine *engine = iter->second;
		// create all resources
		string body = conn->getRequestBody();
		skipWs(&body);
		vector<int> resourceIds;
		while (body.length() > 0) {
			size_t eoln = body.find_first_of("\r\n");
			string line = eoln != string::npos ? body.substr(0, eoln): body;
			if (line.length() > 0) {
				int n = atoi(line.c_str());
				TestResource *tr = dynamic_cast<TestResource*>(engine->CreateResource(resourceId));
				assert(tr != NULL);
				tr->setId(n);
				tr->setStr(line.c_str());
				if (!engine->PutResource(tr, true))
					break;
				resourceIds.push_back(n);
			}
			body = body.substr(eoln);
			skipWs(&body);
		}
		if (body.length() > 0) {
			conn->setResponseCode(500, "Error passing data to ProcessingEngine");
			return true;
		}
			
		// wait for result
		int i = 0;
		while (i < resourceIds.size()) {
			TestResource *tr = dynamic_cast<TestResource*>(engine->GetResource(resourceIds[i], true));
			if (!tr)
				break;
			char s[1024];
			snprintf(s, sizeof(s), "%d %s\r\n", tr->getId(), tr->getStr());
			conn->appendResponseBody(s);
			i++;
		}
		if (i == resourceIds.size())
			conn->setResponseCode(200, "OK");
		else
			conn->setResponseCode(500, "Error getting data from ProcessingEngine");
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

extern "C" SimpleHTTPServer* create(ObjectRegistry *objects, vector<ProcessingEngine*> *engines) {
	return new TestServer(objects, engines);
}
