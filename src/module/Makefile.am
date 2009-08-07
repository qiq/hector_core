AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

lib_LTLIBRARIES = libmodule-dummy.la

libmodule_dummy_la_SOURCES = \
		ModuleDummy.h ModuleDummy.cc
libmodule_dummy_la_CXXFLAGS = $(AM_CXXFLAGS) -I../lib -I../lib/processing_chain
libmodule_dummy_la_LIBADD = -lhector
libmodule_dummy_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS) -L../lib
