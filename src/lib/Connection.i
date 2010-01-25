%module Connection

%{
#undef New // protocol buffers + SWIG interaction
#define SWIG_FILE_WITH_INIT // for Python
#include "Connection.h"
%}

class Connection {
        Connection();
        ~Connection();
};
