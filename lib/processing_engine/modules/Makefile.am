SUBDIRS = perl java python

AM_CXXFLAGS = @LOG4CXX_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resources
AM_LDFLAGS = @LOG4CXX_LIBS@ -module -avoid-version
AM_LIBTOOLFLAGS = --tag=disable-static

modulesdir = $(libdir)/hector/modules
modules_LTLIBRARIES = TestInput.la TestOutput.la TestSimple.la TestMulti.la LoadResource.la SaveResource.la

TestInput_la_SOURCES = TestInput.h TestInput.cc
TestInput_la_LIBADD = ../resources/libTestResource.la

TestOutput_la_SOURCES = TestOutput.h TestOutput.cc
TestOutput_la_LIBADD = ../resources/libTestResource.la

TestSimple_la_SOURCES = TestSimple.h TestSimple.cc
TestSimple_la_LIBADD = ../resources/libTestResource.la

TestMulti_la_SOURCES = TestMulti.h TestMulti.cc
TestMulti_la_LIBADD = ../resources/libTestResource.la

LoadResource_la_SOURCES = LoadResource.h LoadResource.cc
LoadResource_la_CXXFLAGS = $(AM_CXXFLAGS) @PROTOBUF_CFLAGS@
LoadResource_la_LDFLAGS = $(AM_LDFLAGS) @PROTOBUF_LIBS@

SaveResource_la_SOURCES = SaveResource.h SaveResource.cc
SaveResource_la_CXXFLAGS = $(AM_CXXFLAGS) @PROTOBUF_CFLAGS@
SaveResource_la_LDFLAGS = $(AM_LDFLAGS) @PROTOBUF_LIBS@
