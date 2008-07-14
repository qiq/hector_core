/**
 * Virtual class, should be used as a base for HTTP server implementation
 */

#ifndef _SIMPLE_HTTP_HANDLER_
#define _SIMPLE_HTTP_HANDLER_

class SimpleHTTPConn;

class SimpleHTTPHandler {
public:
	SimpleHTTPHandler() {};
	~SimpleHTTPHandler() {};

	virtual bool HandleRequest(SimpleHTTPConn *conn) = 0;
};

#endif
