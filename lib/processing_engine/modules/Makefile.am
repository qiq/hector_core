SUBDIRS = perl java python

AM_CXXFLAGS = @LOG4CXX_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resources
AM_LDFLAGS = @LOG4CXX_LIBS@ -module -avoid-version
AM_LIBTOOLFLAGS = --tag=disable-static

modulesdir = $(libdir)/hector/modules
modules_LTLIBRARIES = TestInput.la TestOutput.la TestSimple.la TestMulti.la Load.la Save.la Dump.la TestCall.la

TestInput_la_SOURCES = TestInput.h TestInput.cc
TestInput_la_LIBADD = ../resources/libTestResource.la

TestOutput_la_SOURCES = TestOutput.h TestOutput.cc
TestOutput_la_LIBADD = ../resources/libTestResource.la

TestSimple_la_SOURCES = TestSimple.h TestSimple.cc
TestSimple_la_LIBADD = ../resources/libTestResource.la

TestMulti_la_SOURCES = TestMulti.h TestMulti.cc
TestMulti_la_LIBADD = ../resources/libTestResource.la

Load_la_SOURCES = Load.h Load.cc
Load_la_CXXFLAGS = $(AM_CXXFLAGS) @PROTOBUF_CFLAGS@
Load_la_LDFLAGS = $(AM_LDFLAGS) @PROTOBUF_LIBS@

Save_la_SOURCES = Save.h Save.cc
Save_la_CXXFLAGS = $(AM_CXXFLAGS) @PROTOBUF_CFLAGS@
Save_la_LDFLAGS = $(AM_LDFLAGS) @PROTOBUF_LIBS@

Dump_la_SOURCES = Dump.h Dump.cc

TestCall_la_SOURCES = TestCall.h TestCall.cc
TestCall_la_LIBADD = ../resources/libTestResource.la
