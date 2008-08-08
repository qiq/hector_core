/**
 * Simple multi-threaded server
 */

#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include "SimpleServer.h"

using namespace log4cxx;
LoggerPtr SimpleServer::logger(Logger::getLogger("lib.SimpleServer"));

SimpleServer::SimpleServer(const char *addr, int port) {
	if (addr == NULL || !inet_aton(addr, &server_addr))
		server_addr.s_addr = INADDR_ANY;
	server_port = port;
	max_threads = 10;
	lock = new CondLock();
	thread_count = 0;
	allowed_client = new stdext::hash_set<string, string_hash>();
	running = false;
}

SimpleServer::~SimpleServer() {
	delete lock;
	delete allowed_client;
}

void SimpleServer::RestrictAccess(const char *addr) {
	string s(addr);
	allowed_client->insert(addr);
}

void SimpleServer::increaseThreadCount() {
	lock->lock();
	while (thread_count >= max_threads)
		lock->waitSend();
	thread_count++;
	lock->unlock();
}

void SimpleServer::decreaseThreadCount() {
	lock->lock();
	thread_count--;
	lock->signalSend();
	lock->unlock();
}

// temporary structure to pass to a new thread
struct thread_info {
	SimpleServer *server;
	int fd;
};

void *http_service_thread(void *ptr) {
	struct thread_info *info = (struct thread_info*)ptr;
	SimpleServer *server = info->server;
	int fd = info->fd;
	delete info;
	server->ServiceThread(fd);
	return NULL;
}

void *http_main_thread(void *ptr) {
	SimpleServer *server = (SimpleServer *)ptr;
	server->MainThread();
	return NULL;
}

void SimpleServer::MainThread() {
	int main_socket;
	running = true;
	struct sockaddr_in addr;

	pthread_detach(pthread_self());

	if ((main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		perror("Cannot open socket");
		return;
	}
	int on = 1;
	if (setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("Cannot set socket reusable");
		return;
	}
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr = server_addr;
	addr.sin_port = htons(server_port);
	if (bind(main_socket, (sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("Cannot bind to socket");
		return;
	}
	if (listen(main_socket, max_threads) == -1) {
		perror("Cannot listen on socket");
		return;
	}

	while (running) {
		sockaddr_in client_addr;
		socklen_t client_addrlen = sizeof(client_addr);

		// wait until thread is available
		increaseThreadCount();
		int fd = accept(main_socket, (sockaddr*)&client_addr, &client_addrlen);
		if (fd == -1) {
			perror("Cannot accept connection");
			return;
		}
		char *client_ip = inet_ntoa((in_addr)client_addr.sin_addr);
		// check whether IP address is allowed
		if (allowed_client->size() > 0) {
			stdext::hash_set<string, string_hash>::iterator iter = allowed_client->find(client_ip);
			// not found -> not allowed
			if (iter == allowed_client->end()) {
				close(fd);
				decreaseThreadCount();
				LOG4CXX_INFO(logger, "Invalid client refused: " << client_ip);
				continue;
			}
		}

		// create thread and a connection object
		struct thread_info *info = new struct thread_info;
		info->server = this;
		info->fd = fd;
		if (pthread_create(&tid, NULL, http_service_thread, (void *)info) != 0) {
			close(fd);
			continue;
		}
	}
}

void SimpleServer::Start(int max_threads) {
	this->max_threads = max_threads;
	pthread_t thread;
	pthread_create(&thread, NULL, http_main_thread, (void *)this);
}

void SimpleServer::Stop() {
	running = false;
}
