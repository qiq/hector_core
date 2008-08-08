/**
 * Simple parser for configuration files based on XML
 * It is split into items, item has flat-config only
 */

#ifndef _CONFIG_PARSER_
#define _CONFIG_PARSER_

#include <limits.h>
#include <ext/hash_map>
#include <string>
#include "common.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

/* FIXME: this is wrong (old) example!
<?xml ?>
<config>
	<module name="main">
		<server-host>*</server-host>
		<server-port>8080</server-port>
		<lib>testlib-1</lib>
		<lib>testlib-2</lib>
		<submodule>module_1</submodule>
		<submodule>module_2</submodule>
	</module>
	<module name="module_1">
		<server-host>*</server-host>
		<server-port>8080</server-port>
		<lib>testlib-1</lib>
		<lib>testlib-2</lib>
	</module>
</config>
*/

/**
 * One entry
 */

class ConfigEntry {
	string name;	// this is redundant :-)
	string value;
	stdext::hash_map<string, string, string_hash> attrs;
	int xmlLine;
	int xmlColumn;
public:
	ConfigEntry(const char *name, int xmlLine = -1, int xmlColumn = -1);
	~ConfigEntry();
	void setName(const char *name);
	const char *getName();
	void setValue(const char *value);
	const char *getValue();
	void setAttr(const char *name, const char *value);
	const char *getAttr(const char *name);
	int getXMLline();
	int getXMLcolumn();
};

class ConfigItem {
	string name;
	string type;
	stdext::hash_map<string, vector<ConfigEntry*> *, string_hash> entries;
	vector<ConfigEntry*> *getEntryVector(const char *name, bool create = false);
public:
	ConfigItem(const char *name);
	~ConfigItem();
	
	void setName(const char *name);
	const char *getName();
	void setType(const char *type);
	const char *getType();

	int addEntry(const char *entryName, int xmlLine = -1, int xmlColumn = -1);
	bool setValue(const char *entryName, const char *entryValue, int index = 0);
	const char *getValue(const char *entryName, int index = 0);
	int getSize(const char *entryName);
	bool setAttr(const char *entryName, const char *attrName, const char *attrValue, int index = 0);
	const char *getAttr(const char *entryName, const char *attrName, int index = 0);
	int getXMLline(const char *entryName, int index = 0);
	int getXMLcolumn(const char *entryName, int index = 0);
};

class Config {
	stdext::hash_map<string, ConfigItem*, string_hash> items;
	ConfigItem *getItem(const char *itemName, bool create = false);
	char *itemName;
	string entryName;
	string entryText;
	int entryIndex;
	void processNode(void *);
public:
	Config();
	~Config();
	bool parseFile(const char *fileName);

	int addEntry(const char *itemName, const char *entryName, int xmlLine = -1, int xmlColumn = -1);
	const char *getType(const char *itemName);
	bool setValue(const char *itemName, const char *entryName, const char *value, int index = 0);
	const char *getValue(const char *itemName, const char *entryName, int index = 0);
	int getValueInt(const char *itemName, const char *entryName, int index = 0);
	int getSize(const char *itemName, const char *entryName);
	bool setAttr(const char *itemName, const char *entryName, const char *attrName, const char *attrValue, int index = 0);
	const char *getAttr(const char *itemName, const char *entryName, const char *attrName, int index = 0);
	int getXMLline(const char *itemName, const char *entryName, int index = 0);
	int getXMLcolumn(const char *itemName, const char *entryName, int index = 0);
};

#endif
