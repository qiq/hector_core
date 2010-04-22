AM_CXXFLAGS = @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine
AM_LDFLAGS = @LOG4CXX_LIBS@
AM_LIBTOOLFLAGS = --tag=disable-static

resourcesdir = $(libdir)/hector/resources
resources_LTLIBRARIES = libTestResource.la TestResource.la

libTestResource_la_SOURCES = TestResource.h TestResource.cc
libTestResource_la_LDFLAGS = $(AM_LDFLAGS) -avoid-version

TestResource_la_SOURCES = TestResourceWrapper.h TestResourceWrapper.cc
TestResource_la_LDFLAGS = $(AM_LDFLAGS) -module -avoid-version
TestResource_la_LIBADD = libTestResource.la

EXTRA_DIST = TestResource.i
