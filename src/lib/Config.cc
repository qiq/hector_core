/**
 * Simple parser for configuration files based on XML
 */

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include "common.h"
#include "Config.h"

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
		xmlChar *item = xmlNodeListGetString(doc, nodes->nodesetval->nodeTab[i]->xmlChildrenNode, 1);
		result->push_back((char*)item);
	        xmlFree(item);
	}
	xmlXPathFreeContext(context);
	xmlXPathFreeObject(nodes);

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
	xmlChar *item = xmlNodeListGetString(doc, nodes->nodesetval->nodeTab[0]->xmlChildrenNode, 1);
	char *result = strdup((char*)item);
	xmlFree(item);

	return result;
}

int Config::getInt(const char *s) {
	int i;
	if (!s || sscanf(s, " %d", &i) != 1)
		return INT_MAX;
	return i;
}
