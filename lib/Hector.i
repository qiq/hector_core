%module Hector

%{
#undef New // protocol buffers + SWIG interaction
#undef die // Perl
#define SWIG_FILE_WITH_INIT // for Python
%}

%include Connection.i
%include StdioConnection.i
%include processing_engine/Resource.i
%include processing_engine/ProtobufResource.i
%include processing_engine/Resources.i
%include processing_engine/resources/TestResource.i
%include processing_engine/resources/TestProtobufResource.i
