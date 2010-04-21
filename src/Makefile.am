AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/server
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@ -L$(top_builddir)/lib

bin_PROGRAMS = server
bin_SCRIPTS = client

server_SOURCES = \
	server.cc

server_CXXFLAGS = $(AM_CXXFLAGS)
server_LDFLAGS = $(AM_LDFLAGS)
server_LDADD = -lhector -lpthread -lltdl

EXTRA_DIST = ${bin_SCRIPTS}
