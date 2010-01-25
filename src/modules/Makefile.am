SUBDIRS = perl java python

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ -I../lib -I../lib/processing_chain -I../resources
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@ -module -avoid-version
AM_LIBTOOLFLAGS = --tag=disable-static

modulesdir = $(pkglibdir)/modules
modules_LTLIBRARIES = DummySimple.la DummyInput.la DummyOutput.la

DummySimple_la_SOURCES = \
		DummySimple.h DummySimple.cc

DummyInput_la_SOURCES = \
		DummyInput.h DummyInput.cc

DummyOutput_la_SOURCES = \
		DummyOutput.h DummyOutput.cc
