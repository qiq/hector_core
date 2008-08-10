/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 * It uses Google Protocol Buffers to de/serialize.
 */

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <log4cxx/logger.h>
#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

class Resource {
	string url;
	uint32_t time;
	char mime_type[50];
	string content;
	vector<string*> header_field;
	vector<string*> extracted_url;
	uint32_t ip_addr;

	static log4cxx::LoggerPtr logger;
public:
	Resource();
	~Resource();
	string *serialize(bool serializeContent = false);
	bool deserialize(string *s);
};

#endif
