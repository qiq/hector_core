
#include "WebResource.h"
#include "WebResource.pb.h"

using namespace std;

log4cxx::LoggerPtr WebResource::logger(log4cxx::Logger::getLogger("lib.processing_chain.WebResource"));

WebResource::WebResource() {
	// check library vs header file versions
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	time = 0;
	mime_type[0] = '\0';
	ip_addr = 0;
}

WebResource::~WebResource() {
	vector<string*>::iterator iter;
	for (iter = header_field.begin(); iter != header_field.end(); ++iter) {
		delete *iter;
	}
	for (iter = extracted_url.begin(); iter != extracted_url.end(); ++iter) {
		delete *iter;
	}
}

int WebResource::getSize() {
	return 1; //FIXME
}

void WebResource::setURL(const char *url) {
	this->url.assign(url);
}

const char *WebResource::getURL() {
	return url.c_str();
}

//xmlURIPtr uri = xmlParseURI(url);
//xmlFreeURI(uri);
string *WebResource::serialize(bool serializeContent) {
	hector::lib::processing_chain::WebResource r;

	r.set_url(url);
	if (time != 0)
		r.set_time(time);
	if (mime_type[0] != '\0')
		r.set_mime_type(mime_type);
	if (serializeContent && content != "")
		r.set_content(content);
	vector<string*>::iterator iter;
	for (iter = header_field.begin(); iter != header_field.end(); ++iter) {
		r.add_header_field(**iter);
	}
	for (iter = extracted_url.begin(); iter != extracted_url.end(); ++iter) {
		r.add_extracted_url(**iter);
	}
	if (ip_addr != 0)
		r.set_ip_addr(ip_addr);

	string *result = new string;
	if (!r.SerializeToString(result)) {
		LOG4CXX_ERROR(logger, "Cannot serialize WebResource");
		delete result;
		return NULL;
	}
	return result;
}

bool WebResource::deserialize(string *s) {
	hector::lib::processing_chain::WebResource r;
	r.ParseFromString(*s);

	url = r.url();
	if (r.has_time())
		time = r.time();
	if (r.has_mime_type())
		strcpy(mime_type, r.mime_type().c_str());
	if (r.has_content())
		content = r.content();
	if (r.header_field_size() > 0) {
		vector<string*>::iterator iter;
		for (iter = header_field.begin(); iter != header_field.end(); ++iter) {
			delete *iter;
		}
		header_field.clear();
		for (int i = 0; i < r.header_field_size(); i++) {
			header_field.push_back(new string(r.header_field(i)));
		}
	}
	if (r.extracted_url_size() > 0) {
		vector<string*>::iterator iter;
		for (iter = extracted_url.begin(); iter != extracted_url.end(); ++iter) {
			delete *iter;
		}
		extracted_url.clear();
		for (int i = 0; i < r.extracted_url_size(); i++) {
			extracted_url.push_back(new string(r.extracted_url(i)));
		}
	}
	if (r.has_ip_addr())
		ip_addr = r.ip_addr();
	return true;
}
