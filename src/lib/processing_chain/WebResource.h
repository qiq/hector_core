/**
 * Class representing queue of resources (mainly HTML pages) while processing.
 * It uses Google Protocol Buffers to de/serialize.
 */

#ifndef _WEB_RESOURCE_H_
#define _WEB_RESOURCE_H_

#include <config.h>

#include <vector>
#include <string>
#include <log4cxx/logger.h>
#include "Resource.h"
#include "WebResource.pb.h"

using namespace std;

class WebResource : public Resource {
protected:
	hector::lib::processing_chain::WebResource r;

	static log4cxx::LoggerPtr logger;
public:
	WebResource();
	~WebResource() {};
	int getStatus();
	void setStatus(int status);
	int getId();
	void setId(int id);
	int getSize();

	void setURL(const char *url);
	string *getURL();

	string *serialize();
	bool deserialize(string *s);

	vector<string> *test();
};

#endif
