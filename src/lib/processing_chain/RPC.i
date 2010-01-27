%include typemaps.i

%{
#include "RPC.h"
%}

class RPC {
public:
        RPC(Connection *connection);
        ~RPC();

        bool Send(int type, int id, string *serialIn);
        bool Send(int type, int *REFERENCE, string *serialIn);
        string *Receive(int *REFERENCE, int *REFERENCE);
        string *SendReceive(int *REFERENCE, string *serialIn);
        bool SendReceiveMessage(RPCMessage *message);
};

%inline %{
        void delete_string(string *s) {
                delete s;
        }
%}
