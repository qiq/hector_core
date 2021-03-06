/**
 * Simple HTTP server for management, one connection
 */

#ifndef _LIB_SERVER_SIMPLE_HTTP_CONN_
#define _LIB_SERVER_SIMPLE_HTTP_CONN_

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
public:
	SimpleHTTPConn(int sock);
	~SimpleHTTPConn();
	void Clear();
	bool IsKeepAlive();

	bool ReadRequest();
	void SendResponse();

	string GetRequestMethod();
	string GetRequestArgs();
	string GetRequestHeaderField(string &field);
	string GetRequestBody();

	void ErrorResponse(int code, const char *description, const char *message);
	void SetResponseCode(int code, const char *description);
	void ClearResponseHeader();
	void AppendResponseHeader(const std::string &s);
	void AppendResponseHeader(const char *s, int size);
	void ClearResponseBody();
	void AppendResponseBody(const std::string &s);
	void AppendResponseBody(const char *s, int size);

private:
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
};

#endif
