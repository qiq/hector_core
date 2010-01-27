%module Hector

%{
#undef New // protocol buffers + SWIG interaction
#define SWIG_FILE_WITH_INIT // for Python
%}

%include Connection.i
%include StdioConnection.i
%include processing_chain/RPC.i
%include processing_chain/RPCMessage.i
%include ../resources/Resources.i
%include ../resources/WebResource.i
