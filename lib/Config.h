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

class Config {
	xmlDocPtr doc;
	xmlNodePtr root;
public:
	Config();
	~Config();
	bool parseFile(const char *fileName, std::vector<std::string> *args);

	std::vector<std::string> *getValues(const char *XPath);
	char *getFirstValue(const char *XPath);
	bool evaluateXPath(const char *XPath);
	int getInt(const char *s);
};

#endif
