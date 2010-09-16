/**
 * Simple parser for configuration files based on XML
 */

#include <config.h>

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

bool Config::parseFile(const char *fileName) {
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

vector<string> *Config::getValues(const char *xpath) {
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
			do {
				if ((node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE) && node->content)
					val += (char*)node->content;
			} while ((node = node->next));
			result->push_back(val);
		} else {
			xmlChar *item = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
			result->push_back(item ? (char*)item : "");
	        	xmlFree(item);
		}
	}
	xmlXPathFreeObject(nodes);
	xmlXPathFreeContext(context);

	return result;
}

char *Config::getFirstValue(const char *xpath) {
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
	char *result;
	xmlNodePtr node = nodes->nodesetval->nodeTab[0];
	if (node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE) {
		string val;
		do {
			if ((node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE) && node->content)
				val += (char*)node->content;
		} while ((node = node->next));
		result = strdup(val.c_str());
	} else {
		xmlChar *item = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
		result = item ? strdup((char*)item) : NULL;
        	xmlFree(item);
	}

	xmlXPathFreeObject(nodes);
	xmlXPathFreeContext(context);

	return result;
}

bool Config::evaluateXPath(const char *xpath) {
	xmlXPathContextPtr context;

	context = xmlXPathNewContext(doc);
	bool result = (xmlXPathEvalExpression((const xmlChar*)xpath, context) != 0);
	xmlXPathFreeContext(context);
	return result;
}

int Config::getInt(const char *s) {
	int i;
	if (!s || sscanf(s, " %d", &i) != 1)
		return INT_MAX;
	return i;
}
