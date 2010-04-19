// 
#include <config.h>

#include "Server.h"
#include "LibraryLoader.h"

log4cxx::LoggerPtr Server::logger(log4cxx::Logger::getLogger("lib.Server"));

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
		LOG_ERROR(logger, "Invalid number of threads, using 1 thread");
		threads = 1;
	}
	free(s);

	// serverHost
	snprintf(buffer, sizeof(buffer), "//Server[@id='%s']/serverHost", getId());
	serverHost = config->getFirstValue(buffer);
	if (!serverHost) {
		LOG_ERROR(logger, "Server/serverHost not found");
		return false;
	}

	// serverPort
	snprintf(buffer, sizeof(buffer), "//Server[@id='%s']/serverPort", getId());
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &serverPort) != 1) {
		LOG_ERROR(logger, "Server/serverPort not found");
		return false;
	}
	free(s);

	// create processing chain(s)
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

	// library
	snprintf(buffer, sizeof(buffer), "//Server[@id='%s']/@lib", getId());
	s = config->getFirstValue(buffer);
	if (!s) {
		LOG_ERROR(logger, "Server/lib not found");
		return false;
	}
	
	// load library
	SimpleHTTPServer *(*create)(ObjectRegistry*) = (SimpleHTTPServer*(*)(ObjectRegistry*))LibraryLoader::loadLibrary(s, "create");
	if (!create) {
		LOG_ERROR(logger, "Invalid library: " << s);
		return false;
	}
	simpleHTTPServer = (*create)(objects);
	free(s);

	return true;
}

void Server::Start(bool wait) {
	// start server
	LOG_INFO(logger, "Starting server " << serverHost << ":" << serverPort << " (" << threads << ")");
	simpleHTTPServer->Start(serverHost, serverPort, threads, wait);
	if (wait) {
		LOG_INFO(logger, "Stopping processing chains");
		for (vector<ProcessingEngine*>::iterator iter = processingEngines.begin(); iter != processingEngines.end(); ++iter) {
			(*iter)->Stop();
		}
		LOG_INFO(logger, "Stopping server");
	}
}

void Server::Stop() {
	LOG_INFO(logger, "Stopping processing chains");
	for (vector<ProcessingEngine*>::iterator iter = processingEngines.begin(); iter != processingEngines.end(); ++iter) {
		(*iter)->Stop();
	}
	LOG_INFO(logger, "Stopping server");
	simpleHTTPServer->Stop();
}

void Server::Pause() {
}

void Server::Resume() {
}