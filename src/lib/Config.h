/**
 * Simple parser for configuration files based on XML
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <config.h>

#include <limits.h>
#include <string>
#include <vector>
#include <libxml/parser.h>
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

class Config {
	xmlDocPtr doc;
	xmlNodePtr root;
public:
	Config();
	~Config();
	bool parseFile(const char *fileName);

	vector<string> *getValues(const char *XPath);
	char *getFirstValue(const char *XPath);
	bool evaluateXPath(const char *XPath);
	int getInt(const char *s);
};

#endif
