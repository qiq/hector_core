/**
 * Simple multi-threaded server
 */

#ifndef _SIMPLE_SERVER_
#define _SIMPLE_SERVER_

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <log4cxx/logger.h>
#include <string>
#include <ext/hash_set>
#include "common.h"
#include "Lock.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

class SimpleServer {
	struct in_addr server_addr;
	int server_port;

	int max_threads;
	CondLock *lock;		// guarding thread_count
	int thread_count;
	pthread_t tid;

	stdext::hash_set<string, string_hash> *allowed_client;
	int running;

	static log4cxx::LoggerPtr logger;
public:
	SimpleServer(const char *addr, int port);
	~SimpleServer();
	void RestrictAccess(const char *addr);
	void Start(int maxThreads);
	void Stop();

	void increaseThreadCount();
	void decreaseThreadCount();
	void MainThread();

	virtual void ServiceThread(int fd) = 0;
};

#endif
