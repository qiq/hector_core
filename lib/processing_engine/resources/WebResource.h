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
	helen::resources::WebResource r;

	static log4cxx::LoggerPtr logger;
public:
	WebResource();
	~WebResource() {};
	Resource *Clone();
	int getId();
	void setId(int id);
	int getStatus();
	void setStatus(int status);
	int getSize();
	resource_t getType();

	void setURL(const char *url);
	const char *getURL();

	string *serialize();
	bool deserialize(string *s);
};

inline int WebResource::getId() {
	return r.id();
}

inline void WebResource::setId(int id) {
	r.set_id(id);
}

inline int WebResource::getStatus() {
	return r.status();
}

inline void WebResource::setStatus(int status) {
	r.set_status(status);
}

inline resource_t WebResource::getType() {
	return RESOURCE_WEB;
}

inline const char *WebResource::getURL() {
	return r.url().c_str();
}

inline void WebResource::setURL(const char *url) {
	r.set_url(url);
}

#endif
