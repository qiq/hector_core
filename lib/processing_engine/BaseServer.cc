/**
 *
 */

#include <assert.h>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include "BaseServer.h"
#include "Object.h"
#include "ProcessingEngine.h"
#include "ResourceInputStreamBinary.h"
#include "ResourceOutputStreamBinary.h"

using namespace std;

log4cxx::LoggerPtr BaseServer::logger(log4cxx::Logger::getLogger("servers.BaseServer"));

BaseServer::BaseServer(ObjectRegistry *objects, vector<ProcessingEngine*> *engines) {
	this->objects = objects;
	this->engines = engines;
}

bool BaseServer::Init(std::vector<std::pair<std::string, std::string> > *params) {
	// second stage?
	if (!params)
		return true;

	if (engines->size() == 0) {
		LOG4CXX_ERROR(logger, "No processing engine in a server");
		return false;
	}
	for (vector<ProcessingEngine*>::iterator iter = engines->begin(); iter != engines->end(); ++iter) {
		name2engine[(*iter)->GetId()] = *iter;
	}
	return true;
}

bool BaseServer::HandleRequest(SimpleHTTPConn *conn) {
	string method = conn->GetRequestMethod();
	string args = conn->GetRequestArgs();
	if (args.substr(0, 1) == "/")
		args = args.substr(1);
	if (method == "GET") {
		if (!args.empty()) {
			size_t dot = args.find_first_of('.');
			if (dot != string::npos) {
				// get value
				string object = args.substr(0, dot);
				string property = args.substr(dot+1);
				char *value = objects->GetObjectProperty(object.c_str(), property.c_str());
				if (value) {
					LOG4CXX_INFO(logger, "GET " << args << " = " << value);
					conn->SetResponseCode(200, "OK");
					conn->AppendResponseBody(value);
				} else {
					LOG4CXX_ERROR(logger, "GET " << args << ": Object not found");
					conn->ErrorResponse(400, "Object/property not found", "");
				}
				free(value);
			} else {
				// list object properties
				Object *object = objects->GetObject(args.c_str());
				if (object) {
					vector<string> *names = object->ListProperties();
					string s;
					for (vector<string>::iterator iter = names->begin(); iter != names->end(); ++iter) {
						conn->AppendResponseBody(iter->c_str());
						conn->AppendResponseBody("\r\n");
						if (s.length() > 0)
							s += ", ";
						s += *iter;
					}
					LOG4CXX_INFO(logger, "GET " << args << " = " << s);
					delete names;
				} else {
					// object not found
					LOG4CXX_ERROR(logger, "GET " << args << ": Object not found");
					conn->ErrorResponse(400, "Object not found", "");
				}
			}
		} else {
			// list objects
			vector<string> *all = objects->GetIds();
			conn->SetResponseCode(200, "OK");
			string s;
			for (vector<string>::iterator iter = all->begin(); iter != all->end(); ++iter) {
				conn->AppendResponseBody(iter->c_str());
				conn->AppendResponseBody("\r\n");
				if (s.length() > 0)
					s += ", ";
				s += *iter;
			}
			LOG4CXX_INFO(logger, "GET " << args << " = " << s);
			delete all;
		}
		return true;
	} else if (method == "SET") {
		if (!args.empty()) {
			size_t dot = args.find_first_of('.');
			if (dot != string::npos) {
				string object = args.substr(0, dot);
				string property = args.substr(dot+1);
				string body = conn->GetRequestBody();
				size_t eoln = body.find_first_of("\r\n");
				string value = eoln != string::npos ? body.substr(0, eoln): body;
				if (objects->SetObjectProperty(object.c_str(), property.c_str(), value.c_str())) {
					LOG4CXX_INFO(logger, "SET " << args << " = " << value);
					conn->SetResponseCode(200, "OK");
				} else {
					LOG4CXX_ERROR(logger, "SET " << args << " = " << value << ": Object/property not found");
					conn->ErrorResponse(400, "Object/property not found", "");
				}
			} else {
				LOG4CXX_ERROR(logger, "SET " << args << ": no property");
			}
		} else {
			// error: no object.property given
			LOG4CXX_ERROR(logger, "SET: no arguments");
			conn->ErrorResponse(400, "No object.property argument given", "");
		}
		return true;
	} else if (method == "PROCESS" || method == "PASS") {
		bool process = method == "PROCESS" ? true : false;
		LOG4CXX_INFO(logger, method << " " << args);
		skipWs(args);
		chomp(args);
		tr1::unordered_map<string, ProcessingEngine*>::iterator iter = name2engine.find(args);
		if (iter == name2engine.end()) {
			LOG4CXX_ERROR(logger, "ProcessingEngine " << args << " not found");
			conn->SetResponseCode(500, "ProcessorEngine not found");
			return true;
		}
		
		ProcessingEngine *engine = iter->second;

		// check that PROCESS/PASS is correct for given PE
		if ((process && !engine->GetOutputQueue()) || (!process && engine->GetOutputQueue())) {
			LOG4CXX_ERROR(logger, "Invalid method for ProcessingEngine: " << method);
			conn->SetResponseCode(500, "Invalid method for ProcessorEngine");
			return true;
		}

		// create all resources
		tr1::unordered_set<int> resourceIds;
		vector<int> resourceIdsOrdered;
		string body = conn->GetRequestBody();
		struct timeval timeout = { 0, 0 };
		ResourceInputStreamBinary stream(body);
		uint32_t typeId;
		while (stream.ReadVarint32(&typeId)) {
			Resource *r = Resource::DeserializeResource(stream, typeId, NULL);
			if (!r) {
				char buf[1024];
				snprintf(buf, sizeof(buf), "Cannot deserialize resource of type %d", typeId);
				conn->SetResponseCode(500, buf);
				return true;
			}
			int id = r->GetId();
			if (!engine->ProcessResource(r, &timeout))
				break;
			resourceIdsOrdered.push_back(id);
			resourceIds.insert(id);
		}

		if (stream.ByteCount() < (int)body.length()) {
			conn->SetResponseCode(500, "Error passing data to ProcessingEngine");
			return true;
		}

		if (process) {
			// wait for result
			vector<Resource*> result;
			engine->GetProcessedResources(&resourceIds, &result, &timeout);
			// reorder back to the original order (we wait for all resources anyway)
			tr1::unordered_map<int, Resource*> map;
			for (int i = 0; i < (int)result.size(); i++) {
				map[result[i]->GetId()] = result[i];
			}
			string response;
			ResourceOutputStreamBinary *stream = new ResourceOutputStreamBinary(&response);
			for (int i = 0; i < (int)resourceIdsOrdered.size(); i++) {
				Resource *r = map[resourceIdsOrdered[i]];
				Resource::SerializeResource(r, *stream, true);
				Resource::GetRegistry()->ReleaseResource(r);
			}
			delete stream;
			conn->AppendResponseBody(response);
			if (result.size() == resourceIdsOrdered.size())
				conn->SetResponseCode(200, "OK");
			else
				conn->SetResponseCode(500, "Error getting data from ProcessingEngine");
		} else {
			conn->SetResponseCode(200, "OK");
		}
		return true;
	} else if (method == "SAVE_CHECKPOINT") {
		LOG4CXX_INFO(logger, method << " " << args);
		for (vector<ProcessingEngine*>::iterator iter = engines->begin(); iter != engines->end(); ++iter) {
			if (!(*iter)->SaveCheckpoint(args.c_str())) {
				conn->SetResponseCode(500, "Error saving checkpoint");
				return true;
			}
		}
		conn->SetResponseCode(200, "OK");
		return true;
	} else if (method == "RESTORE_CHECKPOINT") {
		LOG4CXX_INFO(logger, method << " " << args);
		for (vector<ProcessingEngine*>::iterator iter = engines->begin(); iter != engines->end(); ++iter) {
			if (!(*iter)->RestoreCheckpoint(args.c_str())) {
				conn->SetResponseCode(500, "Error restoring checkpoint");
				return true;
			}
		}
		conn->SetResponseCode(200, "OK");
		return true;
	} else if (method == "SHUTDOWN") {
		LOG4CXX_INFO(logger, "SHUTDOWN");
		conn->SetResponseCode(200, "OK");
		conn->AppendResponseBody("Shutting down\r\n");
		conn->AppendResponseBody(conn->GetRequestArgs().c_str());
		conn->AppendResponseBody("\r\n");

		this->SetRunning(false);
		return true;
	} else {
		return HandleExtension(conn);
	}
}

bool BaseServer::HandleExtension(SimpleHTTPConn *conn) {
	return false;
}

// factory functions

extern "C" SimpleHTTPServer* create(ObjectRegistry *objects, vector<ProcessingEngine*> *engines) {
	return new BaseServer(objects, engines);
}
