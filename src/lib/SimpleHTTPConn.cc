/**
 * we do support:
 * - keepalive
 * we do not support:
 * - multi-line heaer fields
 * - pipelining
 */

#include <sys/select.h>
#include "common.h"
#include "SimpleHTTPConn.h"

SimpleHTTPConn::SimpleHTTPConn(int fd) {
	this->socket = fd;
	keep_alive = false;
	request_body_length = -1;
	request_body_offset = -1;
	header_fields = NULL;
	response_code = 200;
	response_str = "OK";
}

SimpleHTTPConn::~SimpleHTTPConn() {
	close(socket);
	delete header_fields;
}

/**
 * Only make sense after header has been read
 */
bool SimpleHTTPConn::isKeepAlive() {
	return keep_alive;
}

void SimpleHTTPConn::parseRequestHeader() {
	// no data to porcess
	if (request_buffer.length() == 0)
		return;
	// already processed
	if (header_fields != NULL)
		return;
	size_t body_offset_a = request_buffer.find("\r\n\r\n");
	size_t body_offset_b = request_buffer.find("\n\n");
	// header not complete
	if (body_offset_a == string::npos && body_offset_b == string::npos)
		return;
	size_t body_offset = body_offset_a < body_offset_b ? body_offset_a + 4 : body_offset_b + 2;

	// parse first line
	size_t sp1 = request_buffer.find(" ", 0);
	if (sp1 == string::npos)
		return;
	size_t sp2 = request_buffer.find(" ", sp1+1);
	if (sp2 == string::npos)
		return;
	size_t offset_a = request_buffer.find("\r\n", sp2+1);
	size_t offset_b = request_buffer.find("\n", sp2+1);
	if (offset_a == string::npos && offset_b == string::npos)
		return;
	size_t offset = offset_a < offset_b ? offset_a + 2 : offset_b + 1;
	request_type = request_buffer.substr(0, sp1);
	request_path = request_buffer.substr(sp1+1, sp2-(sp1+1));

	// parse header fields, we do not support multi-line fields
	request_body_offset = body_offset;
	header_fields = new stdext::hash_map<string, string, string_hash>();
	while (offset < body_offset) {
		size_t nl_a = request_buffer.find("\r\n", offset);
		size_t nl_b = request_buffer.find("\n", offset);
		if (nl_a == string::npos && nl_b == string::npos)
			return;
		size_t nl = nl_a < nl_b ? nl_a : nl_b;
		// empty line: we are finished
		if (nl - offset == 0)
			break;
		string field = request_buffer.substr(offset, nl-offset);
		offset = nl + (nl_a < nl_b ? 2 : 1);
		size_t col = field.find(": ");
		if (col == string::npos)
			continue;
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
}

bool SimpleHTTPConn::requestReady() {
	if (header_fields == NULL)
		parseRequestHeader();
	if (header_fields == NULL)
		return false;
	// check there is enough data (for keep-alive)
	if (keep_alive) {
		if ((int)request_buffer.length()-request_body_offset < request_body_length)
			return false;
	}
	return true;
}

/**
 * Returns false on error
 */

#define TIMEOUT 30

bool SimpleHTTPConn::readRequest() {
	// TODO: zero everything: for keep-alive
	bool finished = false;
	while (!finished) {
		// set timeout for reading 
		fd_set recv_fd;
		FD_ZERO(&recv_fd);
		FD_SET(socket, &recv_fd);
		struct timeval timeout = { TIMEOUT, 0 };
		int ready = select(socket+1, &recv_fd, NULL, NULL, &timeout);
		if (ready < 0) {
			fprintf(stderr, "Error selecting");
			break;
		}
		if (ready == 0) {
			fprintf(stderr, "Timeout");
			break;
		}

		char buffer[65537];
		int r = read(socket, buffer, sizeof(buffer)-1);
		if (r < 0) {
			fprintf(stderr, "Error reading from socket");
			break;
		}
		buffer[r] = '\0';
		request_buffer += buffer;
	
		finished = requestReady();
	}
	return finished;
}

string SimpleHTTPConn::getRequestType() {
	return request_type;
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
			fprintf(stderr, "Error selecting");
			break;
		}
		if (ready == 0) {
			fprintf(stderr, "Timeout");
			break;
		}
		int w = write(socket, (void *)result.data(), result.length()-send);
		if (w < 0) {
			fprintf(stderr, "Error writing to socket");
			break;
		}
		send += w;

		finished = (send >= (int)result.length());
	}
}
