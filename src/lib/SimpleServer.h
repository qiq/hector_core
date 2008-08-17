/**
 * Simple multi-threaded server
 */

#ifndef _SIMPLE_SERVER_
#define _SIMPLE_SERVER_

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <log4cxx/logger.h>
#include <string>
#include <ext/hash_set>
#include "common.h"
#include "SyncQueue.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

class SimpleServer {
	struct in_addr server_addr;
	int server_port;

	pthread_t main_thread;
	int main_socket;

	int nThreads;
	SyncQueue<int> queue;
	pthread_t *threads;

	Lock running_lock;
	bool running;

	stdext::hash_set<string, string_hash> allowed_client;

	static log4cxx::LoggerPtr logger;
public:
	SimpleServer(const char *addr, int port);
	~SimpleServer();
	bool Running();
	void RestrictAccess(const char *addr);
	void Start(int maxThreads);
	void Stop();

	void ServiceThread();
	void MainCleanup();
	void MainThread();

	virtual void Request(int fd) = 0;
};

#endif
