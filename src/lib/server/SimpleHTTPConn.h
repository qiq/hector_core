/**
 * Simple HTTP server for management, one connection
 */

#ifndef _SIMPLE_HTTP_CONN_
#define _SIMPLE_HTTP_CONN_

#include <config.h>

#include <ext/hash_map>
#include <string>
#include <log4cxx/logger.h>
#include "../common.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

typedef enum {
	INCOMPLETE,
	PARSED,
	FAILED
} request_ready_t;

class SimpleHTTPConn {
	int socket;			// socket used

	// keep-alive support
	bool keep_alive;		// Connection: keep-alive used?
	int request_body_length;	// request body content length

	// request
	string request_method;
	string request_args;
	string request_buffer;		// entire request
	int request_body_offset;	// offset of data
	int request_header_offset;	// offset of second line (header)
	stdext::hash_map<string, string, string_hash> *header_fields;	// parsed header field

	// response
	int response_code;
	string response_str;
	string response_header;
	string response_body;

	request_ready_t parseRequestHeader();
	request_ready_t requestReady();

	static log4cxx::LoggerPtr logger;
public:
	SimpleHTTPConn(int sock);
	~SimpleHTTPConn();
	void clear();
	bool isKeepAlive();

	bool readRequest();
	void sendResponse();

	string getRequestMethod();
	string getRequestArgs();
	string getRequestHeaderField(string &field);
	string getRequestBody();

	void errorResponse(int code, const char *description, const char *message);
	void setResponseCode(int code, const char *description);
	void appendResponseHeader(const char *s, bool clear = false);
	void appendResponseBody(const char *s, bool clear = false);
};

#endif
