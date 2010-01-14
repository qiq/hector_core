AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@

bin_PROGRAMS = server
bin_SCRIPTS = client

server_SOURCES = \
	server.cc

server_CXXFLAGS = -I../lib $(AM_CXXFLAGS)
server_LDFLAGS = $(AM_LDFLAGS)
server_LDADD = -lpthread -L../lib -lhector -lltdl
