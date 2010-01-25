%module RPC

%{
#undef New // protocol buffers + SWIG interaction
#define SWIG_FILE_WITH_INIT // for Python
#include "RPC.h"
%}

class RPC {
        RPC(Connection *connection);
        ~RPC();

        bool Send(int type, int id, string *serialIn);
        bool Send(int type, int *id, string *serialIn);
        string *Receive(int *type, int *id);
        string *SendReceive(int *type, string *serialIn);
        bool SendReceiveMessage(RPCMessage *message);
};
