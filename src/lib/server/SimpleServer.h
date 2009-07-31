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
#include "CondLock.h"
#include "SyncQueue.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

class SimpleServer {
	struct in_addr server_addr;
	int server_port;

	Lock main_lock;
	pthread_t main_thread;		// guarded by main_lock
	bool main_running;		// guarded by main_lock
	int main_socket;		// guarded by main_lock

	int nThreads;
	SyncQueue<int> queue;
	pthread_t *threads;

	stdext::hash_set<string, string_hash> allowed_client;

	static log4cxx::LoggerPtr logger;
public:
	SimpleServer(const char *addr, int port);
	~SimpleServer();
	bool getRunning();
	void setRunning(bool running);
	void RestrictAccess(const char *addr);
	void Start(int maxThreads);
	void ProcessRequests();
	void Stop();

	void ServiceThread();
//	void MainCleanup();
//	void MainThread();

	virtual void Request(int fd) = 0;
};

#endif
