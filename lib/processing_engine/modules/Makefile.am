SUBDIRS = perl java python

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ -I.. -I../.. -I../resources
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@ -module -avoid-version -rpath `pwd`
AM_LIBTOOLFLAGS = --tag=disable-static

modulesdir = $(libdir)/hector/modules
modules_LTLIBRARIES = TestInput.la TestOutput.la TestSimple.la TestMulti.la DummySimple.la DummyInput.la DummyOutput.la

TestInput_la_SOURCES = \
		TestInput.h TestInput.cc

TestOutput_la_SOURCES = \
	TestOutput.h TestOutput.cc

TestSimple_la_SOURCES = \
		TestSimple.h TestSimple.cc

TestMulti_la_SOURCES = \
		TestMulti.h TestMulti.cc

DummySimple_la_SOURCES = \
		DummySimple.h DummySimple.cc

DummyInput_la_SOURCES = \
		DummyInput.h DummyInput.cc

DummyOutput_la_SOURCES = \
		DummyOutput.h DummyOutput.cc
