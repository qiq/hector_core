/**
 * Simple multi-threaded server
 */

#include <config.h>

#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "SimpleServer.h"

log4cxx::LoggerPtr SimpleServer::logger(log4cxx::Logger::getLogger("lib.SimpleServer"));

SimpleServer::SimpleServer() {
	main_thread = -1;
	main_running = false;
	main_socket = -1;

	nThreads = 10;
	threads = NULL;
	queue = new SyncQueue<FileDescriptor>(0, 0);
}

SimpleServer::~SimpleServer() {
	delete[] threads;
	delete queue;
}

// thread safe
bool SimpleServer::getRunning() {
	bool result;
	main_lock.lock();
	result = main_running;
	main_lock.unlock();
	return result;
}

// thread safe
void SimpleServer::setRunning(bool running) {
	main_lock.lock();
	main_running = running;
	if (!running) {
		shutdown(main_socket, SHUT_RDWR);
		close(main_socket);
	}
	main_lock.unlock();
}

void SimpleServer::RestrictAccess(const char *addr) {
	string s(addr);
	allowed_client.insert(addr);
}

void *http_service_thread(void *ptr) {
	SimpleServer *server = (SimpleServer*)ptr;
	server->ServiceThread();
	return NULL;
}

void SimpleServer::ServiceThread() {
	while (getRunning()) {
		FileDescriptor *fd = queue->getItem(true);
		if (!fd)
			break; // cancelled
		Request(fd->fd);
		close(fd->fd);
		delete fd;
	}
}

void *http_main_thread(void *ptr) {
	SimpleServer *server = (SimpleServer *)ptr;
	server->MainThread();
	return NULL;
}

void SimpleServer::MainThread() {
	struct sockaddr_in addr;
	char client_ip[INET6_ADDRSTRLEN];

	// create threads
	threads = new pthread_t[nThreads];
	for (int i = 0; i < nThreads; i++) {
		pthread_create(&threads[i], NULL, http_service_thread, (void *)this);
	}

	if ((main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		LOG4CXX_ERROR(logger, "Cannot open socket" << strerror(errno));
		return;
	}
	int on = 1;
	if (setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		LOG4CXX_ERROR(logger, "Cannot set socket reusable" << strerror(errno));
		return;
	}
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr = server_addr;
	addr.sin_port = htons(server_port);
	if (bind(main_socket, (sockaddr *)&addr, sizeof(addr)) == -1) {
		LOG4CXX_ERROR(logger, "Cannot bind to socket" << strerror(errno));
		return;
	}
	if (listen(main_socket, nThreads) == -1) {
		LOG4CXX_ERROR(logger, "Cannot listen on socket: " << strerror(errno));
		return;
	}

	while (getRunning()) {
		sockaddr_in client_addr;
		socklen_t client_addrlen = sizeof(client_addr);

		int fd;		// must declare it now because of cleanup_push/cleanup_pop
		fd = accept(main_socket, (sockaddr*)&client_addr, &client_addrlen);
		if (!getRunning())
			break;
		if (fd == -1) {
			LOG4CXX_ERROR(logger, "Cannot accept connection: " << strerror(errno));
			break;
		}
		if (!inet_ntop(AF_INET, (void*)&client_addr.sin_addr, client_ip, sizeof(client_ip))) {
			LOG4CXX_ERROR(logger, "Cannot get IP address");
			continue;
		}
		// check whether IP address is allowed
		if (allowed_client.size() > 0) {
			stdext::hash_set<string, string_hash>::iterator iter = allowed_client.find(client_ip);
			// not found -> not allowed
			if (iter == allowed_client.end()) {
				close(fd);
				LOG4CXX_INFO(logger, "Invalid client refused: " << client_ip);
				continue;
			}
		}
		// put request into the queue, it will be serviced as soon as
		// there is a free thread available
		FileDescriptor *pfd = new FileDescriptor(fd);
		if (!queue->putItem(pfd, true)) {
			delete pfd;
			break;	// canceled
		}
	}
	main_lock.lock();
	if (main_socket != -1) {
		shutdown(main_socket, SHUT_RDWR);
		close(main_socket);
		main_socket = -1;
	}
	main_lock.unlock();

	queue->cancelAll();
        for (int i = 0; i < nThreads; i++) {
		pthread_join(threads[i], NULL);
	}
}

void SimpleServer::Start(const char *addr, int port, int max_threads, bool wait) {
	if (addr == NULL || !inet_aton(addr, &server_addr))
		server_addr.s_addr = INADDR_ANY;
	server_port = port;
	this->nThreads = max_threads;
	main_lock.lock();
	pthread_create(&main_thread, NULL, http_main_thread, (void *)this);
	main_running = true;
	main_lock.unlock();

	if (wait)
		pthread_join(main_thread, NULL);
}

void SimpleServer::Stop() {
	main_lock.lock();
	if (main_running) {
		main_running = false;
		if (main_socket != -1) {
			shutdown(main_socket, SHUT_RDWR);
			close(main_socket);
			main_socket = -1;
		}
	}
	main_lock.unlock();
	
	pthread_join(main_thread, NULL);
}
