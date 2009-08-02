AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

bin_PROGRAMS = server

server_SOURCES = \
	server.cc

server_CXXFLAGS = -I../lib $(AM_CXXFLAGS)
server_LDFLAGS = $(AM_LDFLAGS)
server_LDADD = -lpthread -L../lib -lhector -lltdl
