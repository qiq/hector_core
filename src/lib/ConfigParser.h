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
	ConfigEntry(const char *name);
	~ConfigEntry();
	void setName(const char *name);
	const char *getName();
	void setValue(const char *value);
	const char *getValue();
	void setAttr(const char *name, const char *value);
	const char *getAttr(const char *name);
};

class ConfigModule {
	string name;
	stdext::hash_map<string, vector<ConfigEntry*> *, string_hash> entries;
	vector<ConfigEntry*> *getEntryVector(const char *name, bool create = false);
public:
	ConfigModule(const char *name);
	~ConfigModule();
	
	void setName(const char *name);
	const char *getName();

	int addEntry(const char *entryName);
	bool setValue(const char *entryName, const char *entryValue, int index = 0);
	const char *getValue(const char *entryName, int index = 0);
	bool setAttr(const char *entryName, const char *attrName, const char *attrValue, int index = 0);
	const char *getAttr(const char *entryName, const char *attrName, int index = 0);
};

class ConfigParser {
	stdext::hash_map<string, ConfigModule*, string_hash> modules;
	ConfigModule *getModule(const char *moduleName, bool create = false);
	char *moduleName;
	string entryName;
	string entryText;
	int entryIndex;
	void processNode(void *);
public:
	ConfigParser();
	~ConfigParser();
	bool parseFile(const char *fileName);

	int addEntry(const char *moduleName, const char *entryName);
	bool setValue(const char *moduleName, const char *entryName, const char *value, int index = 0);
	const char *getValue(const char *moduleName, const char *entryName, int index = 0);
	bool setAttr(const char *moduleName, const char *entryName, const char *attrName, const char *attrValue, int index = 0);
	const char *getAttr(const char *moduleName, const char *entryName, const char *attrName, int index = 0);
};

#endif
