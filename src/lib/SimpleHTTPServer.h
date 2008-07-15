/**
 * Simple HTTP server for management
 * Only supports HTTP/1.0
 */

#ifndef _SIMPLE_HTTP_SERVER_
#define _SIMPLE_HTTP_SERVER_

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string>
#include <ext/hash_set>
#include "common.h"
#include "Lock.h"
#include "SimpleHTTPHandler.h"
#include "SimpleHTTPConn.h"

using namespace std;
namespace stdext = ::__gnu_cxx;

class SimpleHTTPServer {
	SimpleHTTPHandler *handler;
	struct in_addr server_addr;
	int server_port;

	int max_threads;
	CondLock *lock;		// guarding thread_count
	int thread_count;
	pthread_t tid;

	stdext::hash_set<string, string_hash> *allowed_client;
	int running;
public:
	SimpleHTTPServer(SimpleHTTPHandler *handler, const char *addr, int port);
	~SimpleHTTPServer();
	void setMaxThreads(int max);
	void increaseThreadCount();
	void decreaseThreadCount();
	void RestrictAccess(const char *addr);
	void Serve();
	void HTTPServiceThread(SimpleHTTPConn *conn);
	void Shutdown();
};

#endif
