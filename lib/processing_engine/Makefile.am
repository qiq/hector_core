SUBDIRS = resources modules servers

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/server
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@

noinst_LTLIBRARIES = libhector_processing_engine.la

hectorincludedir = $(includedir)/hector
hectorinclude_HEADERS = Server.h ProcessingEngine.h Processor.h Module.h ResourceRegistry.h Resource.h \
	SharedResource.h SharedResource.i \
	ResourceAttrInfo.h ResourceAttrInfoT.h ResourceAttrInfoP.h \
	ResourceInputStream.h ResourceOutputStream.h \
	PerlResourceInterpreter.h PerlResourceInterpreter.cc PerlInterpreters.h PerlResource.h PerlModule.h \
	PythonModule.h \
	CallProcessingEngine.h BaseServer.h \
	heapplus.h \
	ResourceRegistry.i Resource.i ResourceAttrInfo.i ResourceInputStream.i ResourceOutputStream.i \
	PerlResource.i Module.i

libhector_processing_engine_la_SOURCES = \
	Server.h Server.cc \
	ProcessingEngine.h ProcessingEngine.cc \
	Processor.h Processor.cc \
	Module.h Module.cc \
	OutputFilter.h \
	ResourceRegistry.h ResourceRegistry.cc \
	Resource.h Resource.cc \
	ResourceAttrInfo.h ResourceAttrInfoT.h ResourceAttrInfoP.h \
	PerlResourceInterpreter.h PerlResourceInterpreter.cc PerlInterpreters.h PerlResource.h PerlModule.h \
	PythonModule.h \
	CallProcessingEngine.h CallProcessingEngine.cc \
	BaseServer.h BaseServer.cc

libhector_processing_engine_la_CXXFLAGS = $(AM_CXXFLAGS)
libhector_processing_engine_la_LDFLAGS = $(AM_LDFLAGS)

if WITH_SWIG

if WITH_PERL
libhector_processing_engine_la_SOURCES += PerlModule.cc PerlInterpreters.cc PerlResource.cc
libhector_processing_engine_la_CXXFLAGS += @PERL_CFLAGS@ @PROTOBUF_CFLAGS@
libhector_processing_engine_la_LDFLAGS += @PERL_LDFLAGS@ @PROTOBUF_LIBS@
endif

if WITH_PYTHON
libhector_processing_engine_la_SOURCES += PythonModule.cc
libhector_processing_engine_la_CXXFLAGS += @PYTHON_CFLAGS@
libhector_processing_engine_la_LDFLAGS += @PYTHON_LDFLAGS@
endif

endif

# SWIG interface
EXTRA_DIST = Resource.i ResourceRegistry.i ResourceAttrInfo.i PerlResource.i \
	ResourceInputStream.i ResourceOutputStream.i Module.i
