AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ -I../lib -I../lib/processing_chain
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ -version-info 0:0:0

#AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
#AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

lib_LTLIBRARIES = libmodule_dummy_simple.la libmodule_dummy_input.la libmodule_dummy_output.la

libmodule_dummy_simple_la_SOURCES = \
		ModuleDummySimple.h ModuleDummySimple.cc
#libmodule_dummy_simple_la_CXXFLAGS = $(AM_CXXFLAGS) -I../lib -I../lib/processing_chain
#libmodule_dummy_simple_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS)

libmodule_dummy_input_la_SOURCES = \
		ModuleDummyInput.h ModuleDummyInput.cc
#libmodule_dummy_input_la_CXXFLAGS = $(AM_CXXFLAGS) -I../lib -I../lib/processing_chain
#libmodule_dummy_input_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS)

libmodule_dummy_output_la_SOURCES = \
		ModuleDummyOutput.h ModuleDummyOutput.cc
#libmodule_dummy_output_la_CXXFLAGS = $(AM_CXXFLAGS) -I../lib -I../lib/processing_chain
#libmodule_dummy_output_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS)
