/**
 * Simple parser for configuration files based on XML
 * It is split into modules, modules have flat-config only
 */

#include <libxml/xmlreader.h>
#include "common.h"
#include "Config.h"

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

/****** ConfigItem ******/

ConfigItem::ConfigItem(const char *name) {
	this->name = name;
}

ConfigItem::~ConfigItem() {
}

void ConfigItem::setName(const char *name) {
	this->name = name;
}

const char *ConfigItem::getName() {
	return name.c_str();
}

void ConfigItem::setType(const char *type) {
	this->type = type;
}

const char *ConfigItem::getType() {
	return type.c_str();
}

vector<ConfigEntry*> *ConfigItem::getEntryVector(const char *entryName, bool create) {
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

int ConfigItem::addEntry(const char *entryName) {
	vector<ConfigEntry*> *v = getEntryVector(entryName, true);
	ConfigEntry *e = new ConfigEntry(entryName);
	v->push_back(e);
	return v->size()-1;
}

bool ConfigItem::setValue(const char *entryName, const char *entryValue, int index) {
	vector<ConfigEntry*> *v = getEntryVector(entryName, true);
	if (index >= (int)v->size())
		return false;
	(*v)[index]->setValue(entryValue);
	return true;
}

const char *ConfigItem::getValue(const char *entryName, int index) {
	vector<ConfigEntry*> *v = getEntryVector(entryName);
	if (!v)
		return NULL;
	if (index >= (int)v->size())
		return NULL;
	return (*v)[index]->getValue();
}

int ConfigItem::getSize(const char *entryName) {
	vector<ConfigEntry*> *v = getEntryVector(entryName);
	if (!v)
		return 0;
	return (int)v->size();
}

bool ConfigItem::setAttr(const char *entryName, const char *attrName, const char *attrValue, int index) {
	vector<ConfigEntry*> *v = getEntryVector(entryName, true);
	if (index >= (int)v->size())
		return false;
	(*v)[index]->setAttr(attrName, attrValue);
	return true;
}

const char *ConfigItem::getAttr(const char *entryName, const char *attrName, int index) {
	vector<ConfigEntry*> *v = getEntryVector(entryName);
	if (!v)
		return NULL;
	if (index >= (int)v->size())
		return NULL;
	return (*v)[index]->getAttr(attrName);
}

/****** Config ******/

Config::Config() {
	itemName = NULL;
}

Config::~Config() {
	free(itemName);
}

#ifndef LIBXML_READER_ENABLED
  #error XMLReader not found, install libxml2 2.6+
#endif

#define ELEMENT 1
#define ENDELEMENT 15
#define TEXT 3

void Config::processNode(void *p) {
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
		if (type == ELEMENT) {
			itemName = (char *)xmlTextReaderGetAttribute(reader, (xmlChar *)"name");
			ConfigItem *item = getItem(itemName, true);
			item->setType(name.c_str());
		} else if (type == ENDELEMENT) {
			free(itemName);
			itemName = NULL;
		}
		break;
	case 2:
		if (type == ELEMENT) {
			entryName = name;
			entryText.clear();
			entryIndex = addEntry(itemName, entryName.c_str());
			if (xmlTextReaderMoveToFirstAttribute(reader) == 1) {
				do {
					string attrName = (char *)xmlTextReaderConstName(reader);
					string attrValue = (char *)xmlTextReaderConstValue(reader);
					setAttr(itemName, name.c_str(), attrName.c_str(), attrValue.c_str(), entryIndex);
				} while (xmlTextReaderMoveToNextAttribute(reader) == 1);
			}
		} else if (type == ENDELEMENT) {
			setValue(itemName, name.c_str(), entryText.c_str(), entryIndex);
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

bool Config::parseFile(const char *fileName) {
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

ConfigItem *Config::getItem(const char *itemName, bool create) {
	string name = itemName;
	stdext::hash_map<string, ConfigItem*, string_hash>::iterator iter = items.find(name);
	ConfigItem *m;
	if (iter == items.end()) {
		if (!create)
			return NULL;
		m = new ConfigItem(itemName);
		items[name] = m;
	} else {
		m = iter->second;
	}
	return m;
}

int Config::addEntry(const char *itemName, const char *entryName) {
	ConfigItem *m = getItem(itemName, true);
	return m->addEntry(entryName);
}

const char *Config::getType(const char *itemName) {
	ConfigItem *m = getItem(itemName);
	if (!m)
		return NULL;
	return m->getType();
}

bool Config::setValue(const char *itemName, const char *entryName, const char *value, int index) {
	ConfigItem *m = getItem(itemName, true);
	return m->setValue(entryName, value, index);
}

const char *Config::getValue(const char *itemName, const char *entryName, int index) {
	ConfigItem *m = getItem(itemName);
	if (!m)
		return NULL;
	return m->getValue(entryName, index);
}

int Config::getValueInt(const char *itemName, const char *entryName, int index) {
	ConfigItem *m = getItem(itemName);
	if (!m)
		return INT_MAX;
	const char *s =  m->getValue(entryName, index);
	int i;
	if (sscanf(s, " %d", &i) != 1)
		return INT_MAX;
	return i;
}

bool Config::setAttr(const char *itemName, const char *entryName, const char *attrName, const char *attrValue, int index) {
	ConfigItem *m = getItem(itemName, true);
	return m->setAttr(entryName, attrName, attrValue, index);
}

int Config::getSize(const char *itemName, const char *entryName) {
	ConfigItem *m = getItem(itemName);
	if (!m)
		return 0;
	return m->getSize(entryName);
}

const char *Config::getAttr(const char *itemName, const char *entryName, const char *attrName, int index) {
	ConfigItem *m = getItem(itemName);
	if (!m)
		return NULL;
	return m->getAttr(entryName, attrName, index);
}
