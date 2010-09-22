// 
#include <config.h>

#include "Object.h"

using namespace std;

log4cxx::LoggerPtr Object::logger(log4cxx::Logger::getLogger("lib.Object"));

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

const char *Object::getLogLevelStr() {
	switch (logLevel) {
	case LOG_LEVEL_TRACE:
		return "TRACE";
		break;
	case LOG_LEVEL_DEBUG:
		return "DEBUG";
		break;
	case LOG_LEVEL_INFO:
		return "INFO";
		break;
	case LOG_LEVEL_ERROR:
		return "ERROR";
		break;
	case LOG_LEVEL_FATAL:
		return "FATAL";
		break;
	default:
		return "unknown";
	}
}

bool Object::setLogLevel(const char *logLevel) {
	if (!strcmp(logLevel, "TRACE")) {
		this->logLevel = LOG_LEVEL_TRACE;
	} else if (!strcmp(logLevel, "DEBUG")) {
		this->logLevel = LOG_LEVEL_DEBUG;
	} else if (!strcmp(logLevel, "INFO")) {
		this->logLevel = LOG_LEVEL_INFO;
	} else if (!strcmp(logLevel, "ERROR")) {
		this->logLevel = LOG_LEVEL_ERROR;
	} else if (!strcmp(logLevel, "FATAL")) {
		this->logLevel = LOG_LEVEL_FATAL;
	} else {
		this->logLevel = LOG_LEVEL_TRACE;
		return false;
	}
	return true;
}
