// 
#include <config.h>

#include <log4cxx/level.h>
#include "Object.h"

using namespace std;

char *Object::getValueSync(const char *name) {
	return NULL;
}

bool Object::setValueSync(const char *name, const char *value) {
	return false;
}

bool Object::isInitOnly(const char *name) {
	return false;
}

vector<string> *Object::listNamesSync() {
	return new vector<string>();
}

void Object::SaveCheckpointSync(const char *path, const char *id) {
}

void Object::RestoreCheckpointSync(const char *path, const char *id) {
}

const char *Object::getLogLevelStr(log4cxx::LoggerPtr logger) {
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
		return getLogLevelStr(parent);
	return "unknown";
}

bool Object::setLogLevel(const char *logLevel) {
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
