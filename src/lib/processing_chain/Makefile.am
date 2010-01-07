noinst_LTLIBRARIES = libhector_processing_chain.la

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

libhector_processing_chain_la_SOURCES = \
	ProcessingChain.h ProcessingChain.cc \
	Processor.h Processor.cc \
	Module.h Module.cc \
	Resource.h \
	WebResource.h WebResource.cc WebResource.pb.h WebResource.pb.cc
libhector_processing_chain_la_CXXFLAGS = -I.. $(AM_CXXFLAGS)
libhector_processing_chain_la_LDFLAGS = $(AM_LDFLAGS)

#nobase_include_HEADERS = \
#	Resource.h \
#	ResourceQueue.h

#robot_CXXFLAGS = -I ../lib -I /usr/local/lib $(AM_CXXFLAGS)
#robot_LDFLAGS = -Wl,-export-dynamic -ggdb
#robot_LDADD = -lpthread -L../lib -lhector -lltdl -lprotobuf $(AM_LDFLAGS)

WebResource.pb.cc WebResource.pb.h: WebResource.proto
	$(PROTOC) WebResource.proto --cpp_out=.

clean-local:
	rm -f WebResource.pb.cc WebResource.pb.h
