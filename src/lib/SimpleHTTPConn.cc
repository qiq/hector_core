/**
 * we do support:
 * - keepalive
 * we do not support:
 * - multi-line heaer fields
 * - pipelining
 */

#include <sys/select.h>
#include <stdio.h>
#include "common.h"
#include "SimpleHTTPConn.h"

log4cxx::LoggerPtr SimpleHTTPConn::logger(log4cxx::Logger::getLogger("lib.SimpleHTTPConn"));

SimpleHTTPConn::SimpleHTTPConn(int fd) {
	this->socket = fd;
	header_fields = NULL;
	clear();
}

SimpleHTTPConn::~SimpleHTTPConn() {
	close(socket);
	delete header_fields;
}

void SimpleHTTPConn::clear() {
	keep_alive = false;
	request_body_length = -1;
	request_method.clear();
	request_args.clear();
	request_buffer.clear();
	request_header_offset = -1;
	request_body_offset = -1;
	delete header_fields;
	header_fields = NULL;
	response_code = 200;
	response_str = "OK";
	response_header.clear();
	response_body.clear();
}

/**
 * Only make sense after header has been read
 */
bool SimpleHTTPConn::isKeepAlive() {
	return keep_alive;
}

request_ready_t SimpleHTTPConn::parseRequestHeader() {
	// already processed
	if (header_fields != NULL)
		return PARSED;
	// no data to porcess
	if (request_buffer.length() == 0)
		return INCOMPLETE;

	if (request_header_offset < 0) {
		// at least one line read?
		size_t second_line_a = request_buffer.find("\r\n");
		size_t second_line_b = request_buffer.find("\n");
		// header not complete
		if (second_line_a == string::npos && second_line_b == string::npos)
			return INCOMPLETE;
		size_t eol = second_line_a < second_line_b ? second_line_a : second_line_b;
		request_header_offset = second_line_a < second_line_b ? second_line_a + 2 : second_line_b + 1;

		// parse first line
		size_t sp1 = request_buffer.find(" ", 0);
		if ((int)sp1 >= request_header_offset)
			sp1 = string::npos;
		if (sp1 == string::npos) {
			request_method = request_buffer.substr(0, eol);
		} else {
			request_method = request_buffer.substr(0, sp1);
			request_args = request_buffer.substr(sp1+1, eol-(sp1+1));
		}
	}

	// header complete?
	size_t body_offset_a = request_buffer.find("\r\n\r\n", request_header_offset-2);
	size_t body_offset_b = request_buffer.find("\n\n", request_header_offset-1);
	// header not complete
	if (body_offset_a == string::npos && body_offset_b == string::npos)
		return INCOMPLETE;
	request_body_offset = body_offset_a < body_offset_b ? body_offset_a + 4 : body_offset_b + 2;

	// parse header fields, we do not support multi-line fields
	header_fields = new stdext::hash_map<string, string, string_hash>();
	int offset = request_header_offset;
	while (offset < request_body_offset) {
		size_t nl_a = request_buffer.find("\r\n", offset);
		size_t nl_b = request_buffer.find("\n", offset);
		if (nl_a == string::npos && nl_b == string::npos)
			return FAILED;		// should not happen
		size_t nl = nl_a < nl_b ? nl_a : nl_b;
		// empty line: we are finished
		if (nl - offset == 0)
			break;
		string field = request_buffer.substr(offset, nl-offset);
		offset = nl + (nl_a < nl_b ? 2 : 1);
		size_t col = field.find(": ");
		if (col == string::npos)
			continue;		// invalid line
		string var = field.substr(0, col);
		string val = field.substr(col+2);
		(*header_fields)[var] = val;
	}

	// disable Connection: keep-alive when there is no Content-Length
	stdext::hash_map<string, string, string_hash>::iterator iter = header_fields->find("Connection");
	if (iter != header_fields->end()) {
		if (!strcasecmp(iter->second.c_str(), "keep-alive")) {
			stdext::hash_map<string, string, string_hash>::iterator iter2 = header_fields->find("Content-Length");
			if (iter2 == header_fields->end()) {
				header_fields->erase("Connection");
				keep_alive = false;
			} else {
				request_body_length = atol(iter2->second.c_str());
				keep_alive = true;
			}
		}
	}
	return PARSED;
}

request_ready_t SimpleHTTPConn::requestReady() {
	request_ready_t r = PARSED;
	if (header_fields == NULL)
		r = parseRequestHeader();
	if (r != PARSED)
		return r;
	// check there is enough data (for keep-alive)
	if (keep_alive) {
		if ((int)request_buffer.length()-request_body_offset < request_body_length)
			return INCOMPLETE;
	}
	return PARSED;
}

/**
 * Returns false on error
 */

#define TIMEOUT 30

bool SimpleHTTPConn::readRequest() {
	while (true) {
		// set timeout for reading 
		fd_set recv_fd;
		FD_ZERO(&recv_fd);
		FD_SET(socket, &recv_fd);
		struct timeval timeout = { TIMEOUT, 0 };
		int ready = select(socket+1, &recv_fd, NULL, NULL, &timeout);
		if (ready < 0) {
			LOG4CXX_ERROR(logger, "Error selecting");
			break;
		}
		if (ready == 0) {
			LOG4CXX_INFO(logger, "Timeout");
			break;
		}

		char buffer[65537];
		int r = read(socket, buffer, sizeof(buffer)-1);
		if (r < 0) {
			LOG4CXX_ERROR(logger, "Error reading from socket");
			break;
		}
		buffer[r] = '\0';
		request_buffer += buffer;

		request_ready_t	status = requestReady();
		if (status == PARSED)
			return true;
		if (status == FAILED)
			break;
		// INCOMPLETE
	}
	return false;
}

string SimpleHTTPConn::getRequestMethod() {
	return request_method;
}

string SimpleHTTPConn::getRequestArgs() {
	return request_args;
}

string SimpleHTTPConn::getRequestHeaderField(string &field) {
	if (header_fields == NULL)
		return NULL;
	
	stdext::hash_map<string, string, string_hash>::iterator iter = header_fields->find(field);
	if (iter != header_fields->end())
		return iter->second;
	return NULL;
}

string SimpleHTTPConn::getRequestBody() {
	if (request_body_offset < 0)
		return NULL;
	return request_buffer.substr(request_body_offset);
}

void SimpleHTTPConn::errorResponse(int code, const char *description, const char *message) {
	setResponseCode(code, description);
	char s[1000];
	snprintf(s, sizeof(s), "<html><head><title>%d - %s</title></head><body>%s</body></html>\r\n", code, description, message);
	appendResponseBody(s, true);
}

void SimpleHTTPConn::setResponseCode(int code, const char *description) {
	response_code = code;
	response_str = description;
}

void SimpleHTTPConn::appendResponseHeader(const char *s, bool clear) {
	if (clear)
		response_header.clear();
	response_header.append(s);
	response_header.append("\r\n");
}

void SimpleHTTPConn::appendResponseBody(const char *s, bool clear) {
	if (clear)
		response_body.clear();
	response_body.append(s);
}


void SimpleHTTPConn::sendResponse() {
	// construct response
	char s[1024];
	snprintf(s, sizeof(s), "HTTP/1.0 %d %s\r\n", response_code, response_str.c_str());
	string result(s);
	result.append(response_header);
	if (keep_alive)
		result.append("Connection: keep-alive\r\n");
	snprintf(s, sizeof(s), "Content-Length: %d\r\n\r\n", response_body.length());
	result.append(s);
	result.append(response_body);

	int send = 0;
	bool finished = false;
	while (!finished) {
		fd_set send_fd;
		FD_ZERO(&send_fd);
		FD_SET(socket, &send_fd);
		struct timeval timeout = { TIMEOUT, 0 };
		int ready = select(socket+1, NULL, &send_fd, NULL, &timeout);
		if (ready < 0) {
			LOG4CXX_ERROR(logger, "Error selecting");
			break;
		}
		if (ready == 0) {
			LOG4CXX_INFO(logger, "Timeout");
			break;
		}
		int w = write(socket, (void *)result.data(), result.length()-send);
		if (w < 0) {
			LOG4CXX_ERROR(logger, "Error reading from socket");
			break;
		}
		send += w;

		finished = (send >= (int)result.length());
	}
}
