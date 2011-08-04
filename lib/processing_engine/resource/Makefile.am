SUBDIRS = perl python

AM_CXXFLAGS = @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine
AM_LDFLAGS = @LOG4CXX_LIBS@ @PROTOBUF_LIBS@
AM_LIBTOOLFLAGS = --tag=disable-static

EXTRA_DIST =
BUILT_SOURCES = 
CLEANFILES =
ALIASES =

resourcedir = $(libdir)/hector/resource
resource_LTLIBRARIES = libTestResource.la TestResource.la libTestProtobufResource.la TestProtobufResource.la libMarkerResource.la MarkerResource.la

hectorincludedir = $(includedir)/hector
hectorinclude_HEADERS = TestResource.h TestResource.i \
	TestProtobufResource.h TestProtobufResource.pb.h TestProtobufResource.i \
	MarkerResource.h MarkerResource.i

libTestResource_la_SOURCES = TestResource.h TestResource.cc
libTestResource_la_LDFLAGS = $(AM_LDFLAGS) -avoid-version

TestResource_la_SOURCES = TestResource.cc
TestResource_la_CXXFLAGS = $(AM_CXXFLAGS) -DWRAPPER
TestResource_la_LDFLAGS = $(AM_LDFLAGS) -module -avoid-version
TestResource_la_LIBADD = libTestResource.la

EXTRA_DIST += TestResource.i
ALIASES += TestResource.la

libTestProtobufResource_la_SOURCES = TestProtobufResource.h TestProtobufResource.cc TestProtobufResource.pb.h TestProtobufResource.pb.cc
libTestProtobufResource_la_LDFLAGS = $(AM_LDFLAGS) -avoid-version

BUILT_SOURCES += TestProtobufResource.pb.h TestProtobufResource.pb.cc
TestProtobfuResource.pb.cc TestProtobufResource.pb.h: TestProtobufResource.proto
	$(PROTOC) TestProtobufResource.proto --cpp_out=.

TestProtobufResource_la_SOURCES = TestProtobufResource.cc
TestProtobufResource_la_CXXFLAGS = $(AM_CXXFLAGS) -DWRAPPER
TestProtobufResource_la_LDFLAGS = $(AM_LDFLAGS) -module -avoid-version
TestProtobufResource_la_LIBADD = libTestProtobufResource.la

EXTRA_DIST += TestProtobufResource.i TestProtobufResource.proto
CLEANFILES += TestProtobufResource.pb.h TestProtobufResource.pb.cc
ALIASES += TestProtobufResource.la

libMarkerResource_la_SOURCES = MarkerResource.h MarkerResource.cc
libMarkerResource_la_LDFLAGS = $(AM_LDFLAGS) -avoid-version

MarkerResource_la_SOURCES = MarkerResource.cc
MarkerResource_la_CXXFLAGS = $(AM_CXXFLAGS) -DWRAPPER
MarkerResource_la_LDFLAGS = $(AM_LDFLAGS) -module -avoid-version
MarkerResource_la_LIBADD = libMarkerResource.la

EXTRA_DIST += MarkerResource.i
ALIASES += MarkerResource.la
