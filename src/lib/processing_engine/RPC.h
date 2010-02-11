
// properly locking: read + write lock, SendReceive is atomic

#ifndef _RPC_H_
#define _RPC_H_

#include "Connection.h"
#include "Lock.h"
#include "RPCMessage.h"

class RPC {
private:
	Connection *connection;
	Lock read_lock;
	Lock write_lock;
	int messageId;

	static log4cxx::LoggerPtr logger;

	bool _Send(int type, int id, string *serialIn, bool lock);	// returns success, id is used
	bool _Send(int type, int *id, string *serialIn, bool lock);	// returns success, id is filled
	string *_Receive(int *type, int *id, bool lock);		// returns serialized or NULL, type & id is filled
public:
	RPC(Connection *connection);
	~RPC();

	bool Send(int type, int id, string *serialIn);		// returns success, id is used
	bool Send(int type, int *id, string *serialIn);		// returns success, id is filled
	string *Receive(int *type, int *id);			// returns serialized or NULL, type & id is filled
	string *SendReceive(int *type, string *serialIn);	// returns serialized or NULL, type is filled
	//bool SendMessage(RPCMessage *message);			// returns success
	bool SendReceiveMessage(RPCMessage *message);		// returns success
};

inline bool RPC::Send(int type, int id, string *serialIn) {
	return _Send(type, id, serialIn, true);
}

inline bool RPC::Send(int type, int *id, string *serialIn) {
	return _Send(type, id, serialIn, true);
}

inline string *RPC::Receive(int *type, int *id) {
	return _Receive(type, id, true);
}

#endif
