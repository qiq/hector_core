/**
 * Simple multi-threaded HTTP server for management
 * Supports HTTP/1.0, keep-alive
 * Does not support pipelining, multi-line header fields
 */

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <ext/hash_set>
#include "SimpleHTTPServer.h"

SimpleHTTPServer::SimpleHTTPServer(SimpleHTTPHandler *handler, const char *addr, int port) {
	this->handler = handler;
	if (addr == NULL || !inet_aton(addr, &server_addr))
		server_addr.s_addr = INADDR_ANY;
	server_port = port;
	max_threads = 10;
	lock = new CondLock();
	thread_count = 0;
	allowed_client = new stdext::hash_set<string, string_hash>();
	running = false;
}

SimpleHTTPServer::~SimpleHTTPServer() {
	delete lock;
	delete allowed_client;
}

void SimpleHTTPServer::RestrictAccess(string &addr) {
	allowed_client->insert(addr);
}

void SimpleHTTPServer::setMaxThreads(int max) {
	max_threads = max;
}

void SimpleHTTPServer::increaseThreadCount() {
	lock->lock();
	while (thread_count >= max_threads)
		lock->waitSend();
	thread_count++;
	lock->unlock();
}

void SimpleHTTPServer::decreaseThreadCount() {
	lock->lock();
	thread_count--;
	lock->signalSend();
	lock->unlock();
}

// temporary structure to pass to a new thread
struct thread_info {
	SimpleHTTPServer *server;
	SimpleHTTPConn *conn;
};

void *http_service_thread(void *ptr) {
fprintf(stderr, "new thread ...\n");
	struct thread_info *info = (struct thread_info*)ptr;
	SimpleHTTPServer *server = info->server;
	SimpleHTTPConn *conn = info->conn;
	delete info;
	server->HTTPServiceThread(conn);
	return NULL;
}

void SimpleHTTPServer::HTTPServiceThread(SimpleHTTPConn *conn) {
	pthread_detach(pthread_self());
	while (1) {
fprintf(stderr, "reading ...\n");
		bool result = conn->readRequest();
fprintf(stderr, "reading: %d\n", result ? 1 : 0);
		// timeout or error
		if (!result)
			break;
		string type = conn->getRequestType();
		if (type == "GET") {
			handler->ProcessGET(conn);
		} else if (type == "POST") {
			handler->ProcessPOST(conn);
		} else {
			conn->errorResponse(501, "Not implemented", "Not implemented");
		}
		conn->sendResponse();
		if (!conn->isKeepAlive())
			break; 
	}
	delete conn;
	decreaseThreadCount();
	pthread_exit(0);
}

void SimpleHTTPServer::Server() {
	int main_socket;
	running = true;
	struct sockaddr_in addr;

	if ((main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		fprintf(stderr, "Cannot open socket");
		return;
	}
	int on = 1;
	if (setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		fprintf(stderr, "Cannot set socket reusable");
		return;
	}
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr = server_addr;
	addr.sin_port = htons(server_port);
	if (bind(main_socket, (sockaddr *)&addr, sizeof(addr)) == -1) {
		fprintf(stderr, "Cannot bind to socket");
		return;
	}
	if (listen(main_socket, max_threads) == -1) {
		fprintf(stderr, "Cannot listen on socket");
		return;
	}

	while (running) {
fprintf(stderr, "running...\n");
		sockaddr_in client_addr;
		socklen_t client_addrlen = sizeof(client_addr);

		// wait until thread is available
		increaseThreadCount();
		int fd = accept(main_socket, (sockaddr*)&client_addr, &client_addrlen);
fprintf(stderr, "accept (%d)...\n", fd);
		if (fd == -1) {
			fprintf(stderr, "Cannot accept connection");
			return;
		}
		char *client_ip = inet_ntoa((in_addr)client_addr.sin_addr);
		// check whether IP address is allowed
		if (allowed_client->size() > 0) {
			stdext::hash_set<string, string_hash>::iterator iter = allowed_client->find(client_ip);
			// not found -> not allowed
			if (iter == allowed_client->end()) {
				close(fd);
				continue;
			}
		}

		// create thread and a connection object
		struct thread_info *info = new struct thread_info;
		info->server = this;
		info->conn = new SimpleHTTPConn(fd);
		if (!pthread_create(&tid, NULL, http_service_thread, (void *)info)) {
			close(fd);
			continue;
		}
	}
}

void SimpleHTTPServer::Shutdown() {
	running = false;
}
