// 
#include <config.h>

#include "Object.h"

char *Object::getValueSync(const char *name) {
	return NULL;
}

bool Object::setValueSync(const char *name, const char *value) {
	return false;
}

vector<string> *Object::listNamesSync() {
	return new vector<string>();
}

void Object::SaveCheckpointSync(const char *path, const char *id) {
}

void Object::RestoreCheckpointSync(const char *path, const char *id) {
}

// FIXME: delme
#if 0
void Object::Log(log4cxx::LoggerPtr logger, log_level_t type, string &msg) {
	switch(type) {
	case TRACE:
		LOG4CXX_TRACE(logger, getId() << ": " << msg);
		break;
	case DEBUG:
		LOG4CXX_DEBUG(logger, getId() << ": " << msg);
		break;
	case INFO:
		LOG4CXX_INFO(logger, getId() << ": " << msg);
		break;
	case ERROR:
		LOG4CXX_ERROR(logger, getId() << ": " << msg);
		break;
	case FATAL:
		LOG4CXX_FATAL(logger, getId() << ": " << msg);
		break;
	}
}

void Object::Log(log4cxx::LoggerPtr logger, log_level_t type, const char *msg) {
	switch(type) {
	case TRACE:
		LOG4CXX_TRACE(logger, getId() << ": " << msg);
		break;
	case DEBUG:
		LOG4CXX_DEBUG(logger, getId() << ": " << msg);
		break;
	case INFO:
		LOG4CXX_INFO(logger, getId() << ": " << msg);
		break;
	case ERROR:
		LOG4CXX_ERROR(logger, getId() << ": " << msg);
		break;
	case FATAL:
		LOG4CXX_FATAL(logger, getId() << ": " << msg);
		break;
	}
}
#endif
