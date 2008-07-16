/**
 * Simple parser for configuration files based on XML
 * It is split into modules, modules have flat-config only
 */

#ifndef _CONFIG_PARSER_
#define _CONFIG_PARSER_

#include <ext/hash_map>
#include <string>

using namespace std;
namespace stdext = ::__gnu_cxx;

/*
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
 * Config for one module
 */

class ConfigEntry {
	string name;	// this is redundant :-)
	string value;
	stdext::hash_map<string, string, string_hash> attrs;
public:
	ConfigEntry(string &name);
	~ConfigEntry();
	void setName(string &name);
	string getName();
	void setValue(string &value);
	string getValue();
	void setAttr(string &name, string &value);
	string getAttr(string &name);
};

class ConfigModule {
	string name;
	stdext::hash_map<string, vector<ConfigEntry*> *, string_hash> entries;
	vector<ConfigEntry*> *getEntryVector(string &name, bool create = false);
public:
	ConfigModule(string &name);
	~ConfigModule();
	
	void setName(string &name);
	string getName();

	int addValue(string &entryName, string &entryValue);
	bool setValue(string &entryName, string &entryValue, int index = 0);
	string getValue(string &entryName, int index = 0);
	bool setAttr(string &entryName, string &attrName, string &attrValue, int index = 0);
	string getAttr(string &entryName, string &attrName, int index = 0);
};

class ConfigParser {
	stdext::hash_map<string, ConfigModule*, string_hash> modules;
	ConfigModule *getModule(string &moduleName, bool create = false);
public:
	ConfigParser();
	~ConfigParser();
	bool parse(const char *fileName);

	int addValue(string &moduleName, string &entryName, string &value);
	bool setValue(string &moduleName, string &entryName, string &value, int index = 0);
	string getValue(string &moduleName, string &entryName, int index = 0);
	bool setAttr(string &moduleName, string &entryName, string &attrName, string &attrValue, int index = 0);
	string getAttr(string &moduleName, string &entryName, string &attrName, int index = 0);
};

#endif
