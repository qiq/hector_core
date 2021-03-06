/**
 * Simple multi-threaded server
 */

#ifndef _LIB_SERVER_SIMPLE_SERVER_
#define _LIB_SERVER_SIMPLE_SERVER_

#include <config.h>

#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <tr1/unordered_set>
#include <log4cxx/logger.h>
#include "CondLock.h"
#include "SyncQueue.h"

using namespace std;

class FileDescriptor {
public:
	int fd;

	FileDescriptor(int fd);
	~FileDescriptor() {};
	int GetSize();
};

inline FileDescriptor::FileDescriptor(int fd) {
	this->fd = fd;
}

inline int FileDescriptor::GetSize() {
	return sizeof(fd);
}

class SimpleServer {
public:
	SimpleServer();
	virtual ~SimpleServer();
	bool GetRunning();
	void SetRunning(bool running);
	void RestrictAccess(const char *addr);
	void Start(const char *addr, int port, int maxThreads, bool wait);
	void ProcessRequests();
	void Stop();

	void ServiceThread();
	void MainThread();

	virtual void Request(int fd) = 0;

private:
	struct in_addr server_addr;
	int server_port;

	PlainLock main_lock;
	pthread_t main_thread;		// guarded by main_lock
	bool main_running;		// guarded by main_lock
	int main_socket;		// guarded by main_lock

	int nThreads;
	pthread_t *threads;
	SyncQueue<FileDescriptor> *queue;

	std::tr1::unordered_set<string> allowed_client;

	static log4cxx::LoggerPtr logger;
};

#endif
