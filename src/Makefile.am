AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/server
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ -L$(top_builddir)/lib

bin_PROGRAMS = hector_server
bin_SCRIPTS = hector_client hector_common.sh

hector_server_SOURCES = \
	hector_server.cc

hector_server_CXXFLAGS = $(AM_CXXFLAGS)
hector_server_LDFLAGS = $(AM_LDFLAGS)
hector_server_LDADD = -lhector -lpthread -lltdl

EXTRA_DIST = ${bin_SCRIPTS}
