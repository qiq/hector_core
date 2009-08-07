/**
 *
 */

#include <config.h>

#include "Queue.h"

log4cxx::LoggerPtr Queue::logger(log4cxx::Logger::getLogger("lib.processing_chain.Queue"));

Queue::Queue() {
	int maxItems = 0;
	int maxSize = 0;
}

Queue::~Queue() {
}

bool Queue::Init(Server *server, Config *config, const char *id) {
	char buffer[1024];
	char *s;
	vector<string> *v;

	this->server = server;

	// maxItems
	snprintf(buffer, sizeof(buffer), "/Config/Queue[@id='%s']/maxItems", id);
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &maxItems) != 1) {
		LOG4CXX_ERROR(logger, "Invalid maxItems value");
		return false;
	}
	free(s);

	// maxMemory
	snprintf(buffer, sizeof(buffer), "/Config/Queue[@id='%s']/maxMemory", id);
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &maxMemory) != 1) {
		LOG4CXX_ERROR(logger, "Invalid maxMemory value");
		return false;
	}
	free(s);

	return true;
}

void Queue::Start() {
}

void Queue::Stop() {
	queue.cancelAll();
}

bool Queue::putResource(Resource *resource) {
	return queue->putItem(resource, true);
}

Resource *Queue::getResource() {
	return queue->getItem(true);
}

const char *Queue::getValue(const char *name) {
	return NULL;
}

bool Queue::setValue(const char *name, const char *value) {
	return false;
}
