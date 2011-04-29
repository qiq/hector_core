/**
 * Simple parser for configuration files based on XML
 */

#ifndef _LIB_CONFIG_H_
#define _LIB_CONFIG_H_

#include <config.h>

#include <string>
#include <vector>
#include <libxml/parser.h>
#include "common.h"

class Config {
public:
	Config();
	~Config();
	bool ParseFile(const char *fileName, std::vector<std::string> *args);

	std::vector<std::string> *GetValues(const char *XPath);
	char *GetFirstValue(const char *XPath);
	bool EvaluateXPath(const char *XPath);
	int GetInt(const char *s);

private:
	std::vector<std::string> args;
	xmlDocPtr doc;
	xmlNodePtr root;

	bool SubstituteArgs(std::string *s, int lineno);
};

#endif
