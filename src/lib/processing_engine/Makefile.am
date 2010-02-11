noinst_LTLIBRARIES = libhector_processing_engine.la

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

BUILT_SOURCES =

libhector_processing_engine_la_SOURCES = \
	ProcessingEngine.h ProcessingEngine.cc \
	Processor.h Processor.cc \
	Module.h Module.cc \
	OutputFilter.h \
	Resource.h \
	RPC.h RPC.cc \
	RPCMessage.h RPCMessage.cc RPCMessage.pb.h RPCMessage.pb.cc \
	RPCSimpleModule.h RPCSimpleModule.cc \
	RPCMultiModule.h RPCMultiModule.cc \
	PerlModule.h

libhector_processing_engine_la_CXXFLAGS = -I.. $(AM_CXXFLAGS)
libhector_processing_engine_la_LDFLAGS = $(AM_LDFLAGS)

if WITH_SWIG
if WITH_PERL
libhector_processing_engine_la_SOURCES += \
	PerlModule.cc perlxsi.c
libhector_processing_engine_la_CXXFLAGS += @PERL_CFLAGS@
libhector_processing_engine_la_CFLAGS = ${libhector_processing_engine_la_CXXFLAGS} @PERL_CFLAGS@
libhector_processing_engine_la_LDFLAGS += @PERL_LDFLAGS@
endif
endif

BUILT_SOURCES += RPCMessage.pb.h RPCMessage.pb.cc
RPCMessage.pb.cc RPCMessage.pb.h: RPCMessage.proto
	$(PROTOC) RPCMessage.proto --cpp_out=.

# SWIG interface
EXTRA_DIST = RPC.i RPCMessage.i Resource.i RPCMessage.proto

CLEANFILES = $(BUILT_SOURCES)
