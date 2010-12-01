%module Hector

%{
#undef New // protocol buffers + SWIG interaction
#undef die // Perl
#define SWIG_FILE_WITH_INIT // for Python
%}

%include "std_vector.i"
%include "std_string.i"
%template(StringVector_hector) std::vector<std::string>;

%include Connection.i
%include StdioConnection.i
%include IpAddr.i
%include Object.i
%include ObjectRegistry.i
%include Module.i
%include Resource.i
%include ProtobufResource.i
%include TestResource.i
%include TestProtobufResource.i
