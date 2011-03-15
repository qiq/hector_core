// 
#include <config.h>

#include <log4cxx/level.h>
#include "Object.h"

using namespace std;

char *Object::GetValueSync(const char *name) {
	return NULL;
}

bool Object::SetValueSync(const char *name, const char *value) {
	return false;
}

vector<string> *Object::ListNamesSync() {
	return new vector<string>();
}

bool Object::SaveCheckpointSync(const char *path) {
	return true;
}

bool Object::RestoreCheckpointSync(const char *path) {
	return true;
}

const char *Object::GetLogLevelStr(log4cxx::LoggerPtr logger) {
	log4cxx::LevelPtr level = logger->getLevel();
	if (level == log4cxx::Level::getTrace())
		return "TRACE";
	else if (level == log4cxx::Level::getDebug())
		return "DEBUG";
	else if (level == log4cxx::Level::getInfo())
		return "INFO";
	else if (level == log4cxx::Level::getError())
		return "ERROR";
	else if (level == log4cxx::Level::getFatal())
		return "FATAL";
	log4cxx::LoggerPtr parent = logger->getParent();
	if (parent)
		return GetLogLevelStr(parent);
	return "unknown";
}

bool Object::SetLogLevel(const char *logLevel) {
	if (!strcmp(logLevel, "TRACE")) {
		this->logger->setLevel(log4cxx::Level::getTrace());
	} else if (!strcmp(logLevel, "DEBUG")) {
		this->logger->setLevel(log4cxx::Level::getDebug());
	} else if (!strcmp(logLevel, "INFO")) {
		this->logger->setLevel(log4cxx::Level::getInfo());
	} else if (!strcmp(logLevel, "ERROR")) {
		this->logger->setLevel(log4cxx::Level::getError());
	} else if (!strcmp(logLevel, "FATAL")) {
		this->logger->setLevel(log4cxx::Level::getFatal());
	} else {
		return false;
	}
	return true;
}
