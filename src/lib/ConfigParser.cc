/**
 * Simple parser for configuration files based on XML
 * It is split into modules, modules have flat-config only
 */

#include "common.h"
#include "ConfigParser.h"

ConfigEntry::ConfigEntry(string &name) {
	this->name = name;
}

ConfigEntry::~ConfigEntry() {
}

void ConfigEntry::setName(string &name) {
	this->name = name;
}

string ConfigEntry::getName() {
	return name;
}

void ConfigEntry::setValue(string &value) {
	this->value = value;
}

string ConfigEntry::getValue() {
	return value;
}

void ConfigEntry::setAttr(string &name, string &value) {
	attrs[name] = value;
}

string ConfigEntry::getAttr(string &name) {
	string result;
	stdext::hash_map<string, string, string_hash>::iterator iter = attrs.find(name);
	if (iter != attrs.end())
		result = iter->second;
	return result;
}

/****** ConfigModule ******/

ConfigModule::ConfigModule(string &name) {
	this->name = name;
}

ConfigModule::~ConfigModule() {
}

void ConfigModule::setName(string &name) {
	this->name = name;
}

string ConfigModule::getName() {
	return name;
}

/*void ConfigModule::addEntry(ConfigEntry *entry) {
	string name = entry->getName();
	stdext::hash_map<string, vector<ConfigEntry*>*, string_hash>::iterator iter = entries->find(name);
	vector<ConfigEntry*> *e;
	if (iter == entries->end()) {
		e = new vector<ConfigEntry*>(1);
		entries[name] = e;
	} else {
		e = iter.second;
	}
	e.push_back(entry);
}

ConfigEntry *ConfigModule::getEntry(string &name, int index) {
	stdext::hash_map<string, vector<ConfigEntry*>*, string_hash>::iterator iter = entries->find(name);
	if (iter == entries->end())
		return NULL;
	vector<ConfigEntry*> *e = iter.second;
	if (index >= e.size())
		return NULL;
	return e[index];
}*/

vector<ConfigEntry*> *ConfigModule::getEntryVector(string &name, bool create) {
	stdext::hash_map<string, vector<ConfigEntry*> *, string_hash>::iterator iter = entries.find(name);
	vector<ConfigEntry*> *v;
	if (iter == entries.end()) {
		if (!create)
			return NULL;
		v = new vector<ConfigEntry*>(1);
		entries[name] = v;
	} else {
		v = iter->second;
	}
	return v;
}

int ConfigModule::addValue(string &entryName, string &entryValue) {
	vector<ConfigEntry*> *v = getEntryVector(entryName, true);
	ConfigEntry *e = new ConfigEntry(entryName);
	e->setValue(entryValue);
	v->push_back(e);
	return v->size()-1;
}

bool ConfigModule::setValue(string &entryName, string &entryValue, int index) {
	vector<ConfigEntry*> *v = getEntryVector(entryName, true);
	if (index >= (int)v->size())
		return false;
	(*v)[index]->setValue(entryValue);
	return true;
}

string ConfigModule::getValue(string &entryName, int index) {
	string s;
	vector<ConfigEntry*> *v = getEntryVector(entryName);
	if (!v)
		return s;
	if (index >= (int)v->size())
		return s;
	return (*v)[index]->getValue();
}

bool ConfigModule::setAttr(string &entryName, string &attrName, string &attrValue, int index) {
	vector<ConfigEntry*> *v = getEntryVector(entryName, true);
	if (index >= (int)v->size())
		return false;
	(*v)[index]->setAttr(attrName, attrValue);
	return true;
}

string ConfigModule::getAttr(string &entryName, string &attrName, int index) {
	string s;
	vector<ConfigEntry*> *v = getEntryVector(entryName);
	if (!v)
		return s;
	if (index >= (int)v->size())
		return s;
	return (*v)[index]->getAttr(attrName);
}

/****** ConfigParser ******/

ConfigParser::ConfigParser() {
}

ConfigParser::~ConfigParser() {
}

bool ConfigParser::parse(const char *fileName) {
	return false;
}

ConfigModule *ConfigParser::getModule(string &moduleName, bool create) {
	stdext::hash_map<string, ConfigModule*, string_hash>::iterator iter = modules.find(moduleName);
	ConfigModule *m;
	if (iter == modules.end()) {
		if (!create)
			return NULL;
		m = new ConfigModule(moduleName);
		modules[moduleName] = m;
	} else {
		m = iter->second;
	}
	return m;
}

bool ConfigParser::setValue(string &moduleName, string &entryName, string &value, int index) {
	ConfigModule *m = getModule(moduleName, true);
	return m->setValue(entryName, value);
}

string ConfigParser::getValue(string &moduleName, string &entryName, int index) {
	ConfigModule *m = getModule(moduleName);
	if (!m) {
		string s;
		return s;
	}
	return m->getValue(entryName, index);
}

bool ConfigParser::setAttr(string &moduleName, string &entryName, string &attrName, string &attrValue, int index) {
	ConfigModule *m = getModule(moduleName, true);
	return m->setAttr(entryName, attrName, attrValue, index);
}

string ConfigParser::getAttr(string &moduleName, string &entryName, string &attrName, int index) {
	ConfigModule *m = getModule(moduleName);
	if (!m) {
		string s;
		return s;
	}
	return m->getAttr(entryName, attrName, index);
}
