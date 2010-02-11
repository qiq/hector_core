%module Hector

%{
#undef New // protocol buffers + SWIG interaction
#define SWIG_FILE_WITH_INIT // for Python
%}

%include Connection.i
%include StdioConnection.i
%include processing_engine/RPC.i
%include processing_engine/RPCMessage.i
%include processing_engine/Resource.i
%include ../resources/Resources.i
%include ../resources/WebResource.i
