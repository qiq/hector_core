AM_CXXFLAGS = @LOG4CXX_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine
AM_LDFLAGS = @LOG4CXX_LIBS@
AM_LIBTOOLFLAGS = --tag=disable-static

EXTRA_DIST =
BUILT_SOURCES = 
CLEANFILES =

resourcesdir = $(libdir)/hector/resources
resources_LTLIBRARIES = libTestResource.la TestResource.la libTestProtobufResource.la TestProtobufResource.la

hectorincludedir = $(includedir)/hector
hectorinclude_HEADERS = TestResource.h TestProtobufResource.h TestProtobufResource.pb.h \
	TestResource.i TestProtobufResource.i

libTestResource_la_SOURCES = TestResource.h TestResource.cc
libTestResource_la_LDFLAGS = $(AM_LDFLAGS) -avoid-version

TestResource_la_SOURCES = TestResourceWrapper.h TestResourceWrapper.cc
TestResource_la_LDFLAGS = $(AM_LDFLAGS) -module -avoid-version
TestResource_la_LIBADD = libTestResource.la

EXTRA_DIST += TestResource.i Testresource.proto

libTestProtobufResource_la_SOURCES = TestProtobufResource.h TestProtobufResource.cc TestProtobufResource.pb.h TestProtobufResource.pb.cc
libTestProtobufResource_la_CXXFLAGS = $(AM_CXXFLAGS) @PROTOBUF_CFLAGS@
libTestProtobufResource_la_LDFLAGS = $(AM_LDFLAGS) -avoid-version @PROTOBUF_LIBS@

BUILT_SOURCES += TestProtobufResource.pb.h TestProtobufResource.pb.cc
TestProtobfuResource.pb.cc TestProtobufResource.pb.h: TestProtobufResource.proto
	$(PROTOC) TestProtobufResource.proto --cpp_out=.

TestProtobufResource_la_SOURCES = TestProtobufResourceWrapper.h TestProtobufResourceWrapper.cc
TestProtobufResource_la_CXXFLAGS = $(AM_CXXFLAGS) @PROTOBUF_CFLAGS@
TestProtobufResource_la_LDFLAGS = $(AM_LDFLAGS) -module -avoid-version @PROTOBUF_LIBS@
TestProtobufResource_la_LIBADD = libTestProtobufResource.la

EXTRA_DIST += TestProtobufResource.i TestProtobufResource.proto
CLEANFILES += TestProtobufResource.pb.h TestProtobufResource.pb.cc
