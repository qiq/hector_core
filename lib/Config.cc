/**
 * Simple parser for configuration files based on XML
 */

#include <config.h>

#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include "common.h"
#include "Config.h"

using namespace std;

Config::Config() {
	doc = NULL;
	root = NULL;

	xmlInitParser();
	LIBXML_TEST_VERSION
}

Config::~Config() {
	if (doc)
		xmlFreeDoc(doc);

	xmlCleanupParser();
}

bool Config::ParseFile(const char *fileName, vector<string> *args) {
	// create copy of args
	for (vector<string>::iterator iter = args->begin(); iter != args->end(); ++iter)
		this->args.push_back(*iter);

	// will show line numbers
	xmlLineNumbersDefault(1);

	if (!(doc = xmlParseFile(fileName))) {
		fprintf(stderr, "%s: failed to parse\n", fileName);
		return false;
	}
	
	if (!(root = xmlDocGetRootElement(doc))) {
		fprintf(stderr, "%s: empty document\n", fileName);
		xmlFreeDoc(doc);
		doc = NULL;
		return false;
	}
	
	if (xmlStrcmp(root->name, (const xmlChar *) "Config")) {
		fprintf(stderr, "document of the wrong type, root node != Config");
		xmlFreeDoc(doc);
		doc = NULL;
		return false;
	}

	return true;
}

vector<string> *Config::GetValues(const char *xpath) {
	xmlXPathContextPtr context;
	xmlXPathObjectPtr nodes;

	context = xmlXPathNewContext(doc);
	nodes = xmlXPathEvalExpression((const xmlChar*)xpath, context);

	// not found
	if (!nodes || xmlXPathNodeSetIsEmpty(nodes->nodesetval)) {
		xmlXPathFreeObject(nodes);
		xmlXPathFreeContext(context);
		return NULL;
	}

	// at least one result found
	vector<string> *result = new vector<string>();
	for (int i = 0; i < nodes->nodesetval->nodeNr; i++) {
		xmlNodePtr node = nodes->nodesetval->nodeTab[i];
		if (node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE) {
			string val;
			int line = -1;
			do {
				if ((node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE) && node->content) {
					if (line < 0)
						line = xmlGetLineNo(node);
					val += (char*)node->content;
				}
			} while ((node = node->next));
			if (!SubstituteArgs(&val, line)) {
				free(result);
				result = NULL;
				break;
			}
			result->push_back(val);
		} else {
			xmlChar *item = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
			string val = item ? (char*)item : "";
			if (!SubstituteArgs(&val, xmlGetLineNo(node))) {
				xmlFree(item);
				free(result);
				result = NULL;
				break;
			}
			result->push_back(val);
	        	xmlFree(item);
		}
	}
	xmlXPathFreeObject(nodes);
	xmlXPathFreeContext(context);

	return result;
}

char *Config::GetFirstValue(const char *xpath) {
	xmlXPathContextPtr context;
	xmlXPathObjectPtr nodes;

	context = xmlXPathNewContext(doc);
	nodes = xmlXPathEvalExpression((const xmlChar*)xpath, context);

	// not found
	if (!nodes || xmlXPathNodeSetIsEmpty(nodes->nodesetval)) {
		xmlXPathFreeObject(nodes);
		xmlXPathFreeContext(context);
		return NULL;
	}

	// at least one result found
	char *result = NULL;
	xmlNodePtr node = nodes->nodesetval->nodeTab[0];
	if (node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE) {
		string val;
		int line = -1;
		do {
			if ((node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE) && node->content) {
				if (line < 0)
					line = xmlGetLineNo(node);
				val += (char*)node->content;
			}
		} while ((node = node->next));
		if (SubstituteArgs(&val, line))
			result = strdup(val.c_str());
	} else {
		xmlChar *item = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
		if (item) {
			string val = (char*)item;
			if (SubstituteArgs(&val, xmlGetLineNo(node)))
				result = strdup(val.c_str());
		}
        	xmlFree(item);
	}

	xmlXPathFreeObject(nodes);
	xmlXPathFreeContext(context);

	return result;
}

bool Config::EvaluateXPath(const char *xpath) {
	xmlXPathContextPtr context;

	context = xmlXPathNewContext(doc);
	bool result = (xmlXPathEvalExpression((const xmlChar*)xpath, context) != 0);
	xmlXPathFreeContext(context);
	return result;
}

int Config::GetInt(const char *s) {
	int i;
	if (!s || sscanf(s, " %d", &i) != 1)
		return INT_MAX;
	return i;
}

bool Config::SubstituteArgs(string *s, int lineno) {
	// substitute $N by arguments
	string tmp;
	bool escape = false;
	for (unsigned i = 0; i < s->length(); i++) {
		char c = s->at(i);
		switch (c) {
		case '\\':
			if (!escape) {
				escape = true;
			} else {
				tmp.append("\\");
				escape = false;
			}
			break;
		case '$':
			if (!escape) {
				i++;
				// collect number
				int start = i;
				int n = 0;
				while (i < s->size() && s->at(i) >= '0' && s->at(i) <= '9') {
					n = n*10+(s->at(i)-'0');
					i++;
				}
				if (n == 0 || i-start == 0) {
					fprintf(stderr, "Invalid argument, line %d: %.10s\n", lineno, s->data()+(start >= 8 ? start-8 : 0));
					return false;
				}
				if (n > (int)args.size()) {
					// invalid number -> error
					fprintf(stderr, "Parameter %d missing, line %d: %.10s\n", n, lineno, s->data()+(start >= 8 ? start-8 : 0));
					return false;
				}
				// replace argument
				tmp.append(args.at(n-1));
				i--;
			} else {
				tmp.append("$");
			}
			escape = false;
			break;
		case '\n':
			lineno++;
		default:
			tmp.append(1, c);
			escape = false;
			break;
		}
	}
	*s = tmp;
	return true;
}

