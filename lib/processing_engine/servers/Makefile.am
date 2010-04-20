AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ -module -avoid-version -rpath `pwd`
AM_LIBTOOLFLAGS = --tag=disable-static

noinst_LTLIBRARIES = TestServer.la

TestServer_la_SOURCES = TestServer.h TestServer.cc
TestServer_la_CXXFLAGS = $(AM_CXXFLAGS) -I ../..
TestServer_la_LDFLAGS = $(AM_LDFLAGS)
