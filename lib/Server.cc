// 
#include <config.h>

#include "Server.h"
#include "LibraryLoader.h"

using namespace std;

Server::Server(const char *id) : Object(NULL, id) {
	serverHost = NULL;
	simpleHTTPServer = NULL;
	// not done in constructor
	objects = new ObjectRegistry();
	objects->registerObject(this);
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

	// threads
	snprintf(buffer, sizeof(buffer), "//Server[@id='%s']/threads", getId());
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &threads) != 1) {
		LOG_ERROR("Invalid number of threads, using 1 thread");
		threads = 1;
	}
	free(s);

	// serverHost
	snprintf(buffer, sizeof(buffer), "//Server[@id='%s']/serverHost", getId());
	serverHost = config->getFirstValue(buffer);
	if (!serverHost) {
		LOG_ERROR("Server/serverHost not found");
		return false;
	}

	// serverPort
	snprintf(buffer, sizeof(buffer), "//Server[@id='%s']/serverPort", getId());
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &serverPort) != 1) {
		LOG_ERROR("Server/serverPort not found");
		return false;
	}
	free(s);

	// create processing engine(s)
	snprintf(buffer, sizeof(buffer), "//Server[@id='%s']/ProcessingEngine/@id", getId());
	v = config->getValues(buffer);
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
	s = config->getFirstValue(buffer);
	if (!s) {
		LOG_ERROR("Server/lib not found");
		return false;
	}
	
	// load server library
	SimpleHTTPServer *(*create)(ObjectRegistry*, vector<ProcessingEngine*>*) = (SimpleHTTPServer*(*)(ObjectRegistry*, vector<ProcessingEngine*>*))LibraryLoader::loadLibrary(s, "create");
	if (!create) {
		LOG_ERROR("Invalid library: " << s);
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
	LOG_INFO("Starting server " << serverHost << ":" << serverPort << " (" << threads << ")");
	simpleHTTPServer->Start(serverHost, serverPort, threads, wait);
	if (wait) {
		LOG_INFO("Stopping processing engines");
		for (vector<ProcessingEngine*>::iterator iter = processingEngines.begin(); iter != processingEngines.end(); ++iter) {
			(*iter)->Stop();
		}
		LOG_INFO("Stopping server");
	}
}

void Server::Stop() {
	LOG_INFO("Stopping processing engines");
	for (vector<ProcessingEngine*>::iterator iter = processingEngines.begin(); iter != processingEngines.end(); ++iter) {
		(*iter)->Stop();
	}
	LOG_INFO("Stopping server");
	simpleHTTPServer->Stop();
}

void Server::Pause() {
}

void Server::Resume() {
}
