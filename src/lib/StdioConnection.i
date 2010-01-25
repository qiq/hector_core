%module StdioConnection

%{
#undef New // protocol buffers + SWIG interaction
#define SWIG_FILE_WITH_INIT // for Python
#include "StdioConnection.h"
%}

class StdioConnection {
        StdioConnection();
        ~StdioConnection();
};
