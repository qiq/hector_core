// 
#include <config.h>

#include "Server.h"
#include "LibraryLoader.h"

#include "EmbeddedPython.h"

using namespace std;

Server::Server(const char *id) : Object(NULL, id) {
	serverHost = NULL;
	simpleHTTPServer = NULL;
	// not done in constructor
	objects = new ObjectRegistry();
	objects->RegisterObject(this);
}

Server::~Server() {
	free(serverHost);
	for (vector<ProcessingEngine*>::iterator iter = processingEngines.begin(); iter != processingEngines.end(); ++iter) {
		delete *iter;
	}
	delete simpleHTTPServer;
	delete objects;
	objects = NULL;
}

bool Server::Init(Config *config) {
	char buffer[1024];
	char *s;
	vector<string> *v;
//
//EmbeddedPython *python = EmbeddedPython::GetPython();
//PyGILState_STATE gstate = python->Lock();
//PyObject *x1 = PyImport_ImportModule("Hector");
//if (!x1) {
//        python->LogError();
//        LOG4CXX_ERROR(logger, "Cannot load Hector");
//        return NULL;
//}
//python->Unlock(gstate);
//void *symbol = LibraryLoader::LoadLibrary("_Hector.so", "SWIG_Python_ConvertPtrAndOwn");
//fprintf(stderr, "symbol: %p\n", symbol);
//symbol = LibraryLoader::LoadLibrary("_Hector.so", "SWIG_Python_ConvertPtrAndOwnX");
//fprintf(stderr, "symbol: %p\n", symbol);
//return false;
//

	// threads
	snprintf(buffer, sizeof(buffer), "//Server[@id='%s']/threads", getId());
	s = config->GetFirstValue(buffer);
	if (!s || sscanf(s, "%d", &threads) != 1) {
		LOG_ERROR(this, "Invalid number of threads, using 1 thread");
		threads = 1;
	}
	free(s);

	// serverHost
	snprintf(buffer, sizeof(buffer), "//Server[@id='%s']/serverHost", getId());
	serverHost = config->GetFirstValue(buffer);
	if (!serverHost) {
		LOG_ERROR(this, "Server/serverHost not found");
		return false;
	}

	// serverPort
	snprintf(buffer, sizeof(buffer), "//Server[@id='%s']/serverPort", getId());
	s = config->GetFirstValue(buffer);
	if (!s || sscanf(s, "%d", &serverPort) != 1) {
		LOG_ERROR(this, "Server/serverPort not found");
		return false;
	}
	free(s);

	// create processing engine(s)
	snprintf(buffer, sizeof(buffer), "//Server[@id='%s']/ProcessingEngine/@id", getId());
	v = config->GetValues(buffer);
	if (v) {
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
			const char *peid = iter->c_str();
			ProcessingEngine *p = new ProcessingEngine(objects, peid);
			if (!p->Init(config))
				return false;
			processingEngines.push_back(p);
		}
		delete v;
	}

	// server library
	snprintf(buffer, sizeof(buffer), "//Server[@id='%s']/@lib", getId());
	s = config->GetFirstValue(buffer);
	if (!s) {
		LOG_ERROR(this, "Server/lib not found");
		return false;
	}
	
	// load server library
	SimpleHTTPServer *(*create)(ObjectRegistry*, vector<ProcessingEngine*>*) = (SimpleHTTPServer*(*)(ObjectRegistry*, vector<ProcessingEngine*>*))LibraryLoader::LoadLibrary(s, "create");
	if (!create) {
		LOG_ERROR(this, "Invalid library: " << s);
		return false;
	}
	free(s);
	simpleHTTPServer = (*create)(objects, &processingEngines);
	// TODO: really parse parameters, dummy for now
	vector<pair<string, string> > params;
	if (!simpleHTTPServer->Init(&params))
		return false;

	// second stage
	for (vector<ProcessingEngine*>::iterator iter = processingEngines.begin(); iter != processingEngines.end(); ++iter) {
		if (!(*iter)->Init(NULL))
			return false;
	}
	if (!simpleHTTPServer->Init(NULL))
		return false;
	return true;
}

void Server::Start(bool autostart, bool wait) {
	// start processing engines (if requested, othewise they may be started using hector_client)
	if (autostart) {
		for (vector<ProcessingEngine*>::iterator iter = processingEngines.begin(); iter != processingEngines.end(); ++iter) {
			(*iter)->Start();
		}
	}
	// start server
	LOG_INFO(this, "Starting server " << serverHost << ":" << serverPort << " (" << threads << ")");
	simpleHTTPServer->Start(serverHost, serverPort, threads, wait);
	if (wait) {
		LOG_INFO(this, "Stopping processing engines");
		for (vector<ProcessingEngine*>::iterator iter = processingEngines.begin(); iter != processingEngines.end(); ++iter) {
			(*iter)->Stop();
		}
		LOG_INFO(this, "Stopping server");
	}
}

void Server::Stop() {
	LOG_INFO(this, "Stopping processing engines");
	for (vector<ProcessingEngine*>::iterator iter = processingEngines.begin(); iter != processingEngines.end(); ++iter) {
		(*iter)->Stop();
	}
	LOG_INFO(this, "Stopping server");
	simpleHTTPServer->Stop();
}

void Server::Pause() {
}

void Server::Resume() {
}
