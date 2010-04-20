SUBDIRS = resources modules servers

noinst_LTLIBRARIES = libhelen_processing_engine.la

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

BUILT_SOURCES =

libhelen_processing_engine_la_SOURCES = \
	ProcessingEngine.h ProcessingEngine.cc \
	Processor.h Processor.cc \
	Module.h Module.cc \
	OutputFilter.h \
	Resource.h \
	RPC.h RPC.cc \
	RPCMessage.h RPCMessage.cc RPCMessage.pb.h RPCMessage.pb.cc \
	RPCSimpleModule.h RPCSimpleModule.cc \
	PerlModule.h

libhelen_processing_engine_la_CXXFLAGS = -I.. $(AM_CXXFLAGS)
libhelen_processing_engine_la_LIBADD = resources/libhelen_resources.la
libhelen_processing_engine_la_LDFLAGS = $(AM_LDFLAGS)

if WITH_SWIG
if WITH_PERL
libhelen_processing_engine_la_SOURCES += \
	PerlModule.cc perlxsi.c
libhelen_processing_engine_la_CXXFLAGS += @PERL_CFLAGS@
libhelen_processing_engine_la_CFLAGS = ${libhelen_processing_engine_la_CXXFLAGS} @PERL_CFLAGS@
libhelen_processing_engine_la_LDFLAGS += @PERL_LDFLAGS@
endif
endif

BUILT_SOURCES += RPCMessage.pb.h RPCMessage.pb.cc
RPCMessage.pb.cc RPCMessage.pb.h: RPCMessage.proto
	$(PROTOC) RPCMessage.proto --cpp_out=.

# SWIG interface
EXTRA_DIST = RPC.i RPCMessage.i Resource.i RPCMessage.proto

CLEANFILES = $(BUILT_SOURCES)
