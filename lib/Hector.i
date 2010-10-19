%module Hector

%{
#undef New // protocol buffers + SWIG interaction
#undef die // Perl
#define SWIG_FILE_WITH_INIT // for Python
%}

%include common.i
%include Connection.i
%include StdioConnection.i
%include Object.i
%include ObjectRegistry.i
%include Module.i
%include Resource.i
%include ProtobufResource.i
%include TestResource.i
%include TestProtobufResource.i
