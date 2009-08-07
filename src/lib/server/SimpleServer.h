/**
 * Simple multi-threaded server
 */

#ifndef _SIMPLE_SERVER_
#define _SIMPLE_SERVER_

#include <config.h>

#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <ext/hash_set>
#include <string>
#include <log4cxx/logger.h>
#include "common.h"
#include "CondLock.h"
#include "SyncQueue.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

class FileDescriptor {
public:
	int fd;

	FileDescriptor(int fd);
	~FileDescriptor() {};
	int getSize();
};

inline FileDescriptor::FileDescriptor(int fd) {
	this->fd = fd;
}

inline int FileDescriptor::getSize() {
	return sizeof(fd);
}

class SimpleServer {
	struct in_addr server_addr;
	int server_port;

	Lock main_lock;
	pthread_t main_thread;		// guarded by main_lock
	bool main_running;		// guarded by main_lock
	int main_socket;		// guarded by main_lock

	int nThreads;
	pthread_t *threads;
	SyncQueue<FileDescriptor> *queue;

	stdext::hash_set<string, string_hash> allowed_client;

	static log4cxx::LoggerPtr logger;
public:
	SimpleServer();
	~SimpleServer();
	bool getRunning();
	void setRunning(bool running);
	void RestrictAccess(const char *addr);
	void Start(const char *addr, int port, int maxThreads, bool wait);
	void ProcessRequests();
	void Stop();

	void ServiceThread();
	void MainThread();

	virtual void Request(int fd) = 0;
};

#endif
