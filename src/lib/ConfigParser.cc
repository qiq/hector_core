/**
 * Simple parser for configuration files based on XML
 * It is split into modules, modules have flat-config only
 */

#include <libxml/xmlreader.h>
#include "common.h"
#include "ConfigParser.h"

ConfigEntry::ConfigEntry(const char *name) {
	this->name = name;
}

ConfigEntry::~ConfigEntry() {
}

void ConfigEntry::setName(const char *name) {
	this->name = name;
}

const char *ConfigEntry::getName() {
	return name.c_str();
}

void ConfigEntry::setValue(const char *value) {
	this->value = value;
}

const char *ConfigEntry::getValue() {
	return value.c_str();
}

void ConfigEntry::setAttr(const char *name, const char *value) {
	attrs[name] = value;
}

const char *ConfigEntry::getAttr(const char *name) {
	stdext::hash_map<string, string, string_hash>::iterator iter = attrs.find(name);
	if (iter == attrs.end())
		return NULL;
	return iter->second.c_str();
}

/****** ConfigModule ******/

ConfigModule::ConfigModule(const char *name) {
	this->name = name;
}

ConfigModule::~ConfigModule() {
}

void ConfigModule::setName(const char *name) {
	this->name = name;
}

const char *ConfigModule::getName() {
	return name.c_str();
}

vector<ConfigEntry*> *ConfigModule::getEntryVector(const char *entryName, bool create) {
	string name = entryName;
	stdext::hash_map<string, vector<ConfigEntry*> *, string_hash>::iterator iter = entries.find(name);
	vector<ConfigEntry*> *v;
	if (iter == entries.end()) {
		if (!create)
			return NULL;
		v = new vector<ConfigEntry*>;
		entries[name] = v;
	} else {
		v = iter->second;
	}
	return v;
}

int ConfigModule::addEntry(const char *entryName) {
	vector<ConfigEntry*> *v = getEntryVector(entryName, true);
	ConfigEntry *e = new ConfigEntry(entryName);
	v->push_back(e);
	return v->size()-1;
}

bool ConfigModule::setValue(const char *entryName, const char *entryValue, int index) {
	vector<ConfigEntry*> *v = getEntryVector(entryName, true);
	if (index >= (int)v->size())
		return false;
	(*v)[index]->setValue(entryValue);
	return true;
}

const char *ConfigModule::getValue(const char *entryName, int index) {
	vector<ConfigEntry*> *v = getEntryVector(entryName);
	if (!v)
		return NULL;
	if (index >= (int)v->size())
		return NULL;
	return (*v)[index]->getValue();
}

bool ConfigModule::setAttr(const char *entryName, const char *attrName, const char *attrValue, int index) {
	vector<ConfigEntry*> *v = getEntryVector(entryName, true);
	if (index >= (int)v->size())
		return false;
	(*v)[index]->setAttr(attrName, attrValue);
	return true;
}

const char *ConfigModule::getAttr(const char *entryName, const char *attrName, int index) {
	vector<ConfigEntry*> *v = getEntryVector(entryName);
	if (!v)
		return NULL;
	if (index >= (int)v->size())
		return NULL;
	return (*v)[index]->getAttr(attrName);
}

/****** ConfigParser ******/

ConfigParser::ConfigParser() {
	moduleName = NULL;
}

ConfigParser::~ConfigParser() {
	free(moduleName);
}

#ifndef LIBXML_READER_ENABLED
  #error XMLReader not found, install libxml2 2.6+
#endif

#define ELEMENT 1
#define ENDELEMENT 15
#define TEXT 3

void ConfigParser::processNode(void *p) {
	xmlTextReaderPtr reader = (xmlTextReaderPtr)p;
	const xmlChar *xname = xmlTextReaderConstName(reader);
	if (xname == NULL)
		return;
	string name = (char *)xname;
	const xmlChar *xvalue = xmlTextReaderConstValue(reader);
	string value = xvalue ? (char *)xvalue : "";
	int depth = xmlTextReaderDepth(reader);
	int type = xmlTextReaderNodeType(reader);

	switch (depth) {
	case 1:
		if (name == "module") {
			if (type == ELEMENT) {
				moduleName = (char *)xmlTextReaderGetAttribute(reader, (xmlChar *)"name");
			} else if (type == ENDELEMENT) {
				free(moduleName);
				moduleName = NULL;
			}
		}
		break;
	case 2:
		if (type == ELEMENT) {
			entryName = name;
			entryText.clear();
			entryIndex = addEntry(moduleName, entryName.c_str());
			if (xmlTextReaderMoveToFirstAttribute(reader) == 1) {
				do {
					string attrName = (char *)xmlTextReaderConstName(reader);
					string attrValue = (char *)xmlTextReaderConstValue(reader);
					setAttr(moduleName, name.c_str(), attrName.c_str(), attrValue.c_str(), entryIndex);
				} while (xmlTextReaderMoveToNextAttribute(reader) == 1);
			}
		} else if (type == ENDELEMENT) {
			setValue(moduleName, name.c_str(), entryText.c_str(), entryIndex);
			entryName.clear();
			entryText.clear();
			entryIndex = -1;
		}
		break;
	case 3:
		if (type == TEXT) {
			if (entryName.length() > 0)
				entryText += value;
		}
		break;
	default:
		break;
	}
}

bool ConfigParser::parseFile(const char *fileName) {
	xmlTextReaderPtr reader;
	int ret;

	reader = xmlReaderForFile(fileName, NULL, 0);
	if (reader != NULL) {
		ret = xmlTextReaderRead(reader);
		while (ret == 1) {
			processNode((void *)reader);
			ret = xmlTextReaderRead(reader);
		}
		xmlFreeTextReader(reader);
		if (ret != 0) {
			fprintf(stderr, "%s : failed to parse\n", fileName);
			return false;
		}
	} else {
		fprintf(stderr, "Unable to open %s\n", fileName);
		return false;
	}
	return true;
}

ConfigModule *ConfigParser::getModule(const char *moduleName, bool create) {
	string name = moduleName;
	stdext::hash_map<string, ConfigModule*, string_hash>::iterator iter = modules.find(name);
	ConfigModule *m;
	if (iter == modules.end()) {
		if (!create)
			return NULL;
		m = new ConfigModule(moduleName);
		modules[name] = m;
	} else {
		m = iter->second;
	}
	return m;
}

int ConfigParser::addEntry(const char *moduleName, const char *entryName) {
	ConfigModule *m = getModule(moduleName, true);
	return m->addEntry(entryName);
}

bool ConfigParser::setValue(const char *moduleName, const char *entryName, const char *value, int index) {
	ConfigModule *m = getModule(moduleName, true);
	return m->setValue(entryName, value, index);
}

const char *ConfigParser::getValue(const char *moduleName, const char *entryName, int index) {
	ConfigModule *m = getModule(moduleName);
	if (!m)
		return NULL;
	return m->getValue(entryName, index);
}

bool ConfigParser::setAttr(const char *moduleName, const char *entryName, const char *attrName, const char *attrValue, int index) {
	ConfigModule *m = getModule(moduleName, true);
	return m->setAttr(entryName, attrName, attrValue, index);
}

const char *ConfigParser::getAttr(const char *moduleName, const char *entryName, const char *attrName, int index) {
	ConfigModule *m = getModule(moduleName);
	if (!m)
		return NULL;
	return m->getAttr(entryName, attrName, index);
}
