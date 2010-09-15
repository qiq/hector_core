// template-based value getters/setters implementation

#ifndef _OBJECT_VALUES_
#define _OBJECT_VALUES_

#include <config.h>

#include <string>
#include <tr1/unordered_map>
#include <vector>
#include <log4cxx/logger.h>

template <class T>
class ObjectValues {
public:
	ObjectValues(T *module): module(module) {};
	~ObjectValues() {};

	void addGetter(const char *name, char *(T::*f)(const char*));
	void addSetter(const char *name, void (T::*f)(const char*, const char*));
	bool InitValues(std::vector<std::pair<std::string, std::string> > *params);
	bool InitValue(const char *name, const char *value);

	char *getValueSync(const char *name);
	bool setValueSync(const char *name, const char *value);
	std::vector<std::string> *listNamesSync();

private:
	T *module;

	std::tr1::unordered_map<std::string, char *(T::*)(const char*)> getters;
	std::tr1::unordered_map<std::string, void(T::*)(const char*, const char*)> setters;

	static log4cxx::LoggerPtr logger;
};

template <class T> log4cxx::LoggerPtr ObjectValues<T>::logger(log4cxx::Logger::getLogger("lib.ObjectValues"));


template<class T>
bool ObjectValues<T>::InitValues(std::vector<std::pair<std::string, std::string> > *params) {
	for (std::vector<pair<std::string, std::string> >::iterator iter = params->begin(); iter != params->end(); ++iter) {
		typename std::tr1::unordered_map<std::string, void(T::*)(const char*, const char*)>::iterator iter2 = setters.find(iter->first);
		if (iter2 != setters.end()) {
			(module->*iter2->second)(iter->first.c_str(), iter->second.c_str());
		} else {
			LOG4CXX_ERROR(logger, module->getId() << ": Invalid value name: " << iter->first);
			return false;
		}
	}
	return true;
}

template<class T>
bool ObjectValues<T>::InitValue(const char *name, const char *value) {
	typename std::tr1::unordered_map<std::string, void(T::*)(const char*, const char*)>::iterator iter = setters.find(name);
	if (iter != setters.end()) {
		(module->*iter->second)(iter->first, iter->second);
		return true;
	} else {
		LOG4CXX_ERROR(logger, module->getId() << ": Invalid value name: " << iter->first);
		return false;
	}
}

template<class T>
void ObjectValues<T>::addGetter(const char *name, char *(T::*f)(const char*)) {
	getters[name] = f;
}

template<class T>
void ObjectValues<T>::addSetter(const char *name, void (T::*f)(const char*, const char*)) {
	setters[name] = f;
}

template<class T>
char *ObjectValues<T>::getValueSync(const char *name) {
	typename std::tr1::unordered_map<std::string, char*(T::*)(const char*)>::iterator iter = getters.find(name);
	if (iter != getters.end())
		return (module->*iter->second)(name);
	return NULL;
}

template<class T>
bool ObjectValues<T>::setValueSync(const char *name, const char *value) {
	typename std::tr1::unordered_map<std::string, void(T::*)(const char*, const char*)>::iterator iter = setters.find(name);
	if (iter != setters.end()) {
		(module->*iter->second)(name, value);
		return true;
	}
	return false;
}

template<class T>
std::vector<std::string> *ObjectValues<T>::listNamesSync() {
	std::vector<std::string> *result = new std::vector<std::string>();
	for (typename std::tr1::unordered_map<std::string, char*(T::*)(const char*)>::iterator iter = getters.begin(); iter != getters.end(); ++iter) {
		result->push_back(iter->first);
	}
	return result;
}

#endif
