/**
 * Simple HTTP server for management, one connection
 */

#ifndef _SIMPLE_HTTP_CONN_
#define _SIMPLE_HTTP_CONN_

using namespace std;
namespace stdext = ::__gnu_cxx;

#include <ext/hash_map>
#include <string>
#include "SimpleHTTPHandler.h"

class SimpleHTTPConn {
	int socket;			// socket used

	// keep-alive support
	bool keep_alive;		// Connection: keep-alive used?
	int request_body_length;	// request body content length

	// request
	string request_method;
	string request_path;
	string request_buffer;		// entire request
	int request_body_offset;	// offset of data
	stdext::hash_map<string, string, string_hash> *header_fields;	// parsed header field

	// response
	int response_code;
	string response_str;
	string response_header;
	string response_body;

	void parseRequestHeader();
	bool requestReady();

public:
	SimpleHTTPConn(int sock);
	~SimpleHTTPConn();
	void clear();
	bool isKeepAlive();

	bool readRequest();
	void sendResponse();

	string getRequestMethod();
	string getRequestPath();
	string getRequestHeaderField(string &field);
	string getRequestBody();

	void errorResponse(int code, const char *description, const char *message);
	void setResponseCode(int code, const char *description);
	void appendResponseHeader(const char *s, bool clear = false);
	void appendResponseBody(const char *s, bool clear = false);
};

#endif
