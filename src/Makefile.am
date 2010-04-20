AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@

bin_PROGRAMS = server
bin_SCRIPTS = client

server_SOURCES = \
	server.cc

server_CXXFLAGS = $(AM_CXXFLAGS) -I../lib
server_LDFLAGS = $(AM_LDFLAGS) -L../lib
server_LDADD = -lhector -lpthread -lltdl

EXTRA_DIST = ${bin_SCRIPTS}
