
#include "WebResource.h"
#include "WebResource.pb.h"

using namespace std;

log4cxx::LoggerPtr WebResource::logger(log4cxx::Logger::getLogger("lib.processing_chain.WebResource"));

WebResource::WebResource() {
	// check library vs header file versions
	GOOGLE_PROTOBUF_VERIFY_VERSION;
}

inline int WebResource::getStatus() {
	return r.status();
}

inline void WebResource::setStatus(int status) {
	r.set_status(status);
}

inline int WebResource::getId() {
	return r.id();
}

inline void WebResource::setId(int id) {
	r.set_id(id);
}

int WebResource::getSize() {
	return 1; //FIXME
}

string *WebResource::getURL() {
	return r.mutable_url();
}

void WebResource::setURL(const char *url) {
	r.set_url(url);
}

string *WebResource::serialize() {
	string *result = new string();
	if (!r.SerializeToString(result)) {
		LOG4CXX_ERROR(logger, "Cannot serialize Resource");
		delete result;
		return NULL;
	}
	return result;
}

bool WebResource::deserialize(string *s) {
	r.ParseFromString(*s);
	return true;
}
