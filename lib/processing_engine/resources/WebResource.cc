
#include "WebResource.h"
#include "WebResource.pb.h"

using namespace std;

log4cxx::LoggerPtr WebResource::logger(log4cxx::Logger::getLogger("lib.processing_chain.WebResource"));

WebResource::WebResource() {
	// check library vs header file versions
	GOOGLE_PROTOBUF_VERIFY_VERSION;
}

Resource *WebResource::Clone() {
	return new WebResource(*this);
}

int WebResource::getSize() {
	return 1; //FIXME
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
