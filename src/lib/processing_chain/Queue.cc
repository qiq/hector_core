/**
 *
 */

#include <config.h>

#include "Queue.h"

log4cxx::LoggerPtr Queue::logger(log4cxx::Logger::getLogger("lib.processing_chain.Queue"));

Queue::Queue(ObjectRegistry *objects, const char *id): Object(objects, id) {
	queue = NULL;
}

Queue::~Queue() {
	delete queue;
}

bool Queue::Init(Config *config) {
	char buffer[1024];
	char *s;

	int maxItems = 0;
	int maxMemory = 0;

	// maxItems
	snprintf(buffer, sizeof(buffer), "/Config/Queue[@id='%s']/maxItems", getId());
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &maxItems) != 1) {
		LOG4CXX_ERROR(logger, "Invalid maxItems value");
		return false;
	}
	free(s);

	// maxMemory
	snprintf(buffer, sizeof(buffer), "/Config/Queue[@id='%s']/maxMemory", getId());
	s = config->getFirstValue(buffer);
	if (!s || sscanf(s, "%d", &maxMemory) != 1) {
		LOG4CXX_ERROR(logger, "Invalid maxMemory value");
		return false;
	}
	free(s);

	queue = new SyncQueue<Resource>(maxItems, maxMemory);

	return true;
}

void Queue::Start() {
}

void Queue::Stop() {
	queue->cancelAll();
}

bool Queue::putResource(Resource *resource) {
	return queue->putItem(resource, true);
}

Resource *Queue::getResource() {
	return queue->getItem(true);
}

char *Queue::getValue(const char *name) {
	return NULL;
}

bool Queue::setValue(const char *name, const char *value) {
	return false;
}

vector<string> *Queue::listNames() {
	return new vector<string>();
}
