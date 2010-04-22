%module Hector

%{
#undef New // protocol buffers + SWIG interaction
#define SWIG_FILE_WITH_INIT // for Python
%}

%include Connection.i
%include StdioConnection.i
%include processing_engine/Resource.i
%include processing_engine/Resources.i
%include processing_engine/resources/TestResource.i
