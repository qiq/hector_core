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
	for (vector<ProcessingChain*>::iterator iter = processingChains.begin(); iter != processingChains.end(); ++iter) {
		delete *iter;
	}
	delete simpleHTTPServer;
	delete objects;
	objects = NULL;
}

bool Server::init(Config *config) {
	char buffer[1024];
	char *s;
	vector<string> *v;

	char *baseDir = config->getFirstValue("/Config/@baseDir");
	if (!baseDir) {
		LOG4CXX_ERROR(logger, "Cannot find baseDir");
		return false;
	}

	// threads
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']/threads", getId());
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &threads) != 1) {
		LOG4CXX_ERROR(logger, "Invalid number of threads, using 1 thread");
		threads = 1;
	}
	free(s);

	// serverHost
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']/serverHost", getId());
	serverHost = config->getFirstValue(buffer);
	if (!serverHost) {
		LOG4CXX_ERROR(logger, "Server/serverHost not found");
		return false;
	}

	// serverPort
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']/serverPort", getId());
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &serverPort) != 1) {
		LOG4CXX_ERROR(logger, "Server/serverPort not found");
		return false;
	}
	free(s);

	// create processing chain(s)
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']/processingChain/@ref", getId());
	v = config->getValues(buffer);
	if (v) {
		for (vector<string>::iterator iter = v->begin(); iter != v->end(); ++iter) {
			const char *pid = iter->c_str();
			ProcessingChain *p = new ProcessingChain(objects, pid);
			if (!p->init(config))
				return false;
			processingChains.push_back(p);
		}
		delete v;
	}

	// library
	snprintf(buffer, sizeof(buffer), "/Config/Server[@id='%s']/lib/@name", getId());
	s = config->getFirstValue(buffer);
	if (!s) {
		LOG4CXX_ERROR(logger, "Server/lib not found");
		return false;
	}
	
	// load library
	snprintf(buffer, sizeof(buffer), "%s/%s", baseDir, s);
	SimpleHTTPServer *(*create)(ObjectRegistry*) = (SimpleHTTPServer*(*)(ObjectRegistry*))LibraryLoader::loadLibrary(buffer, "create");
	if (!create) {
		LOG4CXX_ERROR(logger, "Invalid library: " << buffer);
		return false;
	}
	simpleHTTPServer = (*create)(objects);
	free(s);

	free(baseDir);
	return true;
}

void Server::start(bool wait) {
	// start server
	LOG4CXX_INFO(logger, "Starting server " << serverHost << ":" << serverPort << " (" << threads << ")");
	simpleHTTPServer->start(serverHost, serverPort, threads, wait);
	if (wait) {
		LOG4CXX_INFO(logger, "Stopping processing chains");
		for (vector<ProcessingChain*>::iterator iter = processingChains.begin(); iter != processingChains.end(); ++iter) {
			(*iter)->stop();
		}
		LOG4CXX_INFO(logger, "Stopping server");
	}
}

void Server::stop() {
	LOG4CXX_INFO(logger, "Stopping processing chains");
	for (vector<ProcessingChain*>::iterator iter = processingChains.begin(); iter != processingChains.end(); ++iter) {
		(*iter)->stop();
	}
	LOG4CXX_INFO(logger, "Stopping server");
	simpleHTTPServer->stop();
}

void Server::pause() {
}

void Server::resume() {
}

char *Server::getValue(const char *name) {
	return NULL;
}

bool Server::setValue(const char *name, const char *value) {
	return false;
}

vector<string> *Server::listNames() {
	return new vector<string>();
}
