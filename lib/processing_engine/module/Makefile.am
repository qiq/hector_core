SUBDIRS = perl java python

AM_CXXFLAGS = @LOG4CXX_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resource
AM_LDFLAGS = @LOG4CXX_LIBS@ -module -avoid-version
AM_LIBTOOLFLAGS = --tag=disable-static

BUILT_SOURCES =

moduledir = $(libdir)/hector/module
module_LTLIBRARIES =

module_LTLIBRARIES += TestInput.la
TestInput_la_SOURCES = TestInput.h TestInput.cc
TestInput_la_CXXFLAGS = $(AM_CXXFLAGS) @PROTOBUF_CFLAGS@
TestInput_la_LDFLAGS = $(AM_LDFLAGS) @PROTOBUF_LIBS@
TestInput_la_LIBADD = ../resource/libTestResource.la ../resource/libTestProtobufResource.la

module_LTLIBRARIES += TestOutput.la
TestOutput_la_SOURCES = TestOutput.h TestOutput.cc
TestOutput_la_LIBADD = ../resource/libTestResource.la

module_LTLIBRARIES += TestSimple.la
TestSimple_la_SOURCES = TestSimple.h TestSimple.cc
TestSimple_la_LIBADD = ../resource/libTestResource.la

module_LTLIBRARIES += TestMulti.la
TestMulti_la_SOURCES = TestMulti.h TestMulti.cc
TestMulti_la_LIBADD = ../resource/libTestResource.la

module_LTLIBRARIES += Load.la
Load_la_SOURCES = Load.h Load.cc
Load_la_CXXFLAGS = $(AM_CXXFLAGS) @PROTOBUF_CFLAGS@
Load_la_LDFLAGS = $(AM_LDFLAGS) @PROTOBUF_LIBS@
Load_la_LIBADD = ../resource/libMarkerResource.la

module_LTLIBRARIES += Save.la
Save_la_SOURCES = Save.h Save.cc
Save_la_CXXFLAGS = $(AM_CXXFLAGS) @PROTOBUF_CFLAGS@
Save_la_LDFLAGS = $(AM_LDFLAGS) @PROTOBUF_LIBS@

module_LTLIBRARIES += Dump.la
Dump_la_SOURCES = Dump.h Dump.cc

module_LTLIBRARIES += TestCall.la
TestCall_la_SOURCES = TestCall.h TestCall.cc
TestCall_la_LIBADD = ../resource/libTestResource.la

module_LTLIBRARIES += Null.la
Null_la_SOURCES = Null.h Null.cc
