AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ -I.. -I../..
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

BUILT_SOURCES =
EXTRA_DIST =

noinst_LTLIBRARIES = libhelen_resources.la
libhelen_resources_la_SOURCES = \
	Resources.h ResourceList.h \
	TestResource.pb.h TestResource.pb.cc TestResource.h TestResource.cc \
	WebResource.pb.h WebResource.pb.cc WebResource.h WebResource.cc

BUILT_SOURCES += WebResource.pb.h WebResource.pb.cc
EXTRA_DIST += WebResource.i WebResource.proto
WebResource.pb.cc WebResource.pb.h: WebResource.proto
	$(PROTOC) WebResource.proto --cpp_out=.

BUILT_SOURCES += TestResource.pb.h TestResource.pb.cc
EXTRA_DIST += TestResource.i TestResource.proto
TestResource.pb.cc TestResource.pb.h: TestResource.proto
	$(PROTOC) TestResource.proto --cpp_out=.

# SWIG interface
EXTRA_DIST += Resources.i

CLEANFILES = $(BUILT_SOURCES)
