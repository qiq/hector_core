AM_CXXFLAGS = @LOG4CXX_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resources -I$(top_srcdir)/lib/server
AM_LDFLAGS = @LOG4CXX_LIBS@ -module -avoid-version
AM_LIBTOOLFLAGS = --tag=disable-static


serverdir = $(libdir)/hector
server_LTLIBRARIES = TestServer.la

TestServer_la_SOURCES = TestServer.h TestServer.cc
TestServer_la_CXXFLAGS = $(AM_CXXFLAGS)
TestServer_la_LDFLAGS = $(AM_LDFLAGS)
TestServer_la_LIBADD = ../resources/libTestResource.la
