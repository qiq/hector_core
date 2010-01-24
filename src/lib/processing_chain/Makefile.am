SUBDIRS = . perl java python

noinst_LTLIBRARIES = libhector_processing_chain.la

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@

BUILT_SOURCES =

libhector_processing_chain_la_SOURCES = \
	ProcessingChain.h ProcessingChain.cc \
	Processor.h Processor.cc \
	Module.h Module.cc \
	OutputFilter.h \
	RPCMessage.h RPCMessage.cc RPCMessage.pb.h RPCMessage.pb.cc \
	ModuleRPCSimple.h ModuleRPCSimple.cc
libhector_processing_chain_la_CXXFLAGS = -I.. -I../../resources $(AM_CXXFLAGS)
libhector_processing_chain_la_LDFLAGS = $(AM_LDFLAGS)

BUILT_SOURCES += RPCMessage.pb.h RPCMessage.pb.cc
RPCMessage.pb.cc RPCMessage.pb.h: RPCMessage.proto
	$(PROTOC) RPCMessage.proto --cpp_out=.

# SWIG interface
EXTRA_DIST = RPCMessage.i

CLEANFILES = $(BUILT_SOURCES)
