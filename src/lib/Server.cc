// 
#include <config.h>

#include "Server.h"

log4cxx::LoggerPtr Server::logger(log4cxx::Logger::getLogger("lib.Server"));

Server::Server() {
	serverHost = NULL;
	processingChains = NULL;
	simpleHTTPServer = NULL;
}

Server::~Server() {
	free(serverHost);
	for (vector<ProcessingChain*>::iterator iter = processingChains.begin(); iter != processingChain.end(); iter++) {
		delete *iter;
	}
	delete simpleHTTPServer;
}

bool Server::Init(Config *config, const char *id) {
	char buffer[1024];
	char *s;
	vector<string> *v;

	char *baseDir = config->getFirstValue("/Config/@baseDir");
	if (!baseDir) {
		LOG4CXX_ERROR(logger, "Cannot find baseDir");
		return false;
	}

	// threads
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']/threads", id);
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &threads) != 1) {
		LOG4CXX_ERROR(logger, "Invalid number of threads, using 1 thread");
		threads = 1;
	}
	free(s);

	// serverHost
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']/serverHost", id);
	serverHost = config->getFirstValue(buffer);
	if (!serverHost) {
		LOG4CXX_ERROR(logger, "Server/serverHost not found");
		return false;
	}

	// serverPort
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']/serverPort", id);
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &serverPort) != 1) {
		LOG4CXX_ERROR(logger, "Server/serverPort not found");
		return false;
	}
	free(s);

	// create processing chain(s)
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']/processingChain/@ref", id);
	v = config->getValues(buffer);
	for (vector<string>::iterator iter = v->begin(); iter != v->end(); iter++) {
		const char *pid = iter->c_str();
		ProcessingChain *p = new ProcessingChain();
		if (!p->Init(this, config, pid))
			return false;
		processingChains.push_back(p);
	}
	delete v;

	// library
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']/lib/@name", id);
	s = config->getFirstValue(buffer);
	if (!s) {
		LOG4CXX_ERROR(logger, "Server/lib not found");
		return false;
	}
	
	// load library
	snprintf(buffer, sizeof(buffer), "%s/%s", baseDir, s);
	SimpleHTTPServer *(*create)(Server*) = (SimpleHTTPServer*(*)(Server*))loadLibrary(buffer, "create");
	if (!create) {
		LOG4CXX_ERROR(logger, "Invalid library: " << buffer);
		return false;
	}
	simpleHTTPServer = (*create)(this);
	free(s);

	free(baseDir);
	return true;
}

void Server::Start(bool wait) {
	// start server
	LOG4CXX_INFO(logger, "Starting server " << serverHost << ":" << serverPort << " (" << threads << ")");
	simpleHTTPServer->Start(serverHost, serverPort, threads, true);
	if (wait)
		LOG4CXX_INFO(logger, "Stopping server");
}

void Server::Stop() {
	LOG4CXX_INFO(logger, "Stopping server");
	simpleHTTPServer->Stop();
}

void Server::registerObject(Object *obj) {
	objects[obj->getId()] = obj;
}


bool Server::unregisterObject(const char *id) {
	stdext::hash_map<const char*, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		objects.erase(id);
		return true;
	}
	return false;
}

const char *Server::getObjectValue(const char *id, const char *name) {
	stdext::hash_map<const char*, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		Object *obj = iter->second;;
		return obj->getValue(name);
	}
	return NULL;
}

bool Server::setObjectValue(const char *id, const char *name, const char *value) {
	stdext::hash_map<const char*, Object*>::iterator iter = objects.find(id);
	if (iter != objects.end()) {
		Object *obj = iter->second;;
		return obj->setValue(name, value);
	}
	return false;
}
