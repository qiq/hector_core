/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 * It uses Google Protocol Buffers to de/serialize.
 */

#ifndef _WEB_RESOURCE_H_
#define _WEB_RESOURCE_H_

#include <log4cxx/logger.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "Resource.h"

using namespace std;

class WebResource : public Resource {
	string url;
	uint32_t time;
	char mime_type[50];
	string content;
	vector<string*> header_field;
	vector<string*> extracted_url;
	uint32_t ip_addr;

	static log4cxx::LoggerPtr logger;
public:
	WebResource();
	~WebResource();
	int getSize();
	void setURL(const char *url);
	const char *getURL();
	string *serialize(bool serializeContent);
	bool deserialize(string *s);
};

#endif
