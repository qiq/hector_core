/* template-based value getters/setters implementation
 *
 * N.B.: properties must be locked when accessed from Process* methods. Anyway,
 * init-only properties that are not changed outside Init() need not to be locked.
 */

#ifndef _LIB_OBJECT_VALUES_
#define _LIB_OBJECT_VALUES_

#include <config.h>

#include <string>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <utility>
#include <vector>
#include <log4cxx/logger.h>

template <class T>
class ObjectValues {
public:
	ObjectValues(T *module): module(module) {};
	~ObjectValues() {};

	void AddGetter(const char *name, char *(T::*f)(const char*));
	void AddSetter(const char *name, void (T::*f)(const char*, const char*), bool initOnly = false);
	bool InitValues(std::vector<std::pair<std::string, std::string> > *params, bool ignoreUnknown = false);
	bool InitValue(const char *name, const char *value, bool ignoreUnknown = false);

	char *GetValue(const char *name);
	bool SetValue(const char *name, const char *value);
	std::vector<std::string> *ListNames();

private:
	T *module;

	std::tr1::unordered_map<std::string, char *(T::*)(const char*)> getters;
	std::tr1::unordered_map<std::string, void(T::*)(const char*, const char*)> setters;
	std::tr1::unordered_map<std::string, void(T::*)(const char*, const char*)> initOnly;

	static log4cxx::LoggerPtr logger;
};

template <class T> log4cxx::LoggerPtr ObjectValues<T>::logger(log4cxx::Logger::getLogger("lib.ObjectValues"));

template<class T>
bool ObjectValues<T>::InitValues(std::vector<std::pair<std::string, std::string> > *params, bool ignoreUnknown) {
	for (std::vector<std::pair<std::string, std::string> >::iterator iter = params->begin(); iter != params->end(); ++iter) {
		typename std::tr1::unordered_map<std::string, void(T::*)(const char*, const char*)>::iterator iter2 = setters.find(iter->first);
		if (iter2 != setters.end()) {
			(module->*iter2->second)(iter->first.c_str(), iter->second.c_str());
		} else {
			typename std::tr1::unordered_map<std::string, void(T::*)(const char*, const char*)>::iterator iter2 = initOnly.find(iter->first);
			if (iter2 != initOnly.end()) {
				(module->*iter2->second)(iter->first.c_str(), iter->second.c_str());
			} else if (!ignoreUnknown) {
				LOG4CXX_ERROR(logger, module->getId() << ": Invalid value name: " << iter->first);
				return false;
			}
		}
	}
	return true;
}

template<class T>
bool ObjectValues<T>::InitValue(const char *name, const char *value, bool ignoreUnknown) {
	typename std::tr1::unordered_map<std::string, void(T::*)(const char*, const char*)>::iterator iter = setters.find(name);
	if (iter != setters.end()) {
		(module->*iter->second)(name, value);
		return true;
	} else {
		typename std::tr1::unordered_map<std::string, void(T::*)(const char*, const char*)>::iterator iter = initOnly.find(name);
		if (iter != initOnly.end()) {
			(module->*iter->second)(name, value);
			return true;
		} else if (!ignoreUnknown) {
			LOG4CXX_ERROR(logger, module->getId() << ": Invalid value name: " << iter->first);
			return false;
		}
	}
}

template<class T>
void ObjectValues<T>::AddGetter(const char *name, char *(T::*f)(const char*)) {
	getters[name] = f;
}

// initOnly: cannot set value outside of InitValue()/InitValues() -- does not require locking at all
template<class T>
void ObjectValues<T>::AddSetter(const char *name, void (T::*f)(const char*, const char*), bool initOnly) {
	if (!initOnly)
		setters[name] = f;
	else
		this->initOnly[name] = f;
}

template<class T>
char *ObjectValues<T>::GetValue(const char *name) {
	typename std::tr1::unordered_map<std::string, char*(T::*)(const char*)>::iterator iter = getters.find(name);
	if (iter != getters.end())
		return (module->*iter->second)(name);
	return NULL;
}

template<class T>
bool ObjectValues<T>::SetValue(const char *name, const char *value) {
	typename std::tr1::unordered_map<std::string, void(T::*)(const char*, const char*)>::iterator iter = setters.find(name);
	if (iter != setters.end()) {
		(module->*iter->second)(name, value);
		return true;
	}
	return false;
}

template<class T>
std::vector<std::string> *ObjectValues<T>::ListNames() {
	std::vector<std::string> *result = new std::vector<std::string>();
	for (typename std::tr1::unordered_map<std::string, char*(T::*)(const char*)>::iterator iter = getters.begin(); iter != getters.end(); ++iter) {
		result->push_back(iter->first);
	}
	return result;
}

#endif
