/**
 * Simple HTTP server for management, one connection
 */

#ifndef _SIMPLE_HTTP_CONN_
#define _SIMPLE_HTTP_CONN_

#include <config.h>

#include <string>
#include <tr1/unordered_map>
#include <log4cxx/logger.h>

using namespace std;

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
	string request_protocol;
	string request_buffer;		// entire request
	int request_body_offset;	// offset of data
	int request_header_offset;	// offset of second line (header)
	std::tr1::unordered_map<string, string> *header_fields;	// parsed header field

	// response
	int response_code;
	string response_str;
	string response_header;
	string response_body;

	request_ready_t ParseRequestHeader();
	request_ready_t RequestReady();

	static log4cxx::LoggerPtr logger;
public:
	SimpleHTTPConn(int sock);
	~SimpleHTTPConn();
	void Clear();
	bool isKeepAlive();

	bool ReadRequest();
	void SendResponse();

	string getRequestMethod();
	string getRequestArgs();
	string getRequestHeaderField(string &field);
	string getRequestBody();

	void ErrorResponse(int code, const char *description, const char *message);
	void setResponseCode(int code, const char *description);
	void appendResponseHeader(const char *s, bool clear = false);
	void appendResponseBody(const char *s, bool clear = false);
};

#endif
