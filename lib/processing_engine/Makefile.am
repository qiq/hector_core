SUBDIRS = resources modules servers

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/server
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

noinst_LTLIBRARIES = libhector_processing_engine.la

hectorincludedir = $(includedir)/hector
hectorinclude_HEADERS = Server.h ProcessingEngine.h Processor.h Module.h ResourceRegistry.h Resource.h \
	ProtobufResource.h ResourceAttrInfo.h ResourceAttrInfoT.h \
	ResourceRegistry.i Resource.i ProtobufResource.i \
	Module.i CallProcessingEngine.h BaseServer.h

BUILT_SOURCES =

libhector_processing_engine_la_SOURCES = \
	Server.h Server.cc \
	ProcessingEngine.h ProcessingEngine.cc \
	Processor.h Processor.cc \
	Module.h Module.cc \
	OutputFilter.h \
	ResourceRegistry.h ResourceRegistry.cc \
	Resource.h Resource.cc ProtobufResource.h ProtobufResource.cc \
	ResourceAttrInfo.h ResourceAttrInfoT.h ResourceAttrInfoP.h \
	PerlResourceInterpreter.h PerlResourceInterpreter.cc rPerlInterpreters.h PerlResource.h PerlModule.h \
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
EXTRA_DIST = Resource.i ProtobufResource.i Module.i

CLEANFILES = $(BUILT_SOURCES)
