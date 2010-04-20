noinst_LTLIBRARIES = libhelen_server.la

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

libhelen_server_la_SOURCES = \
		SimpleServer.h SimpleServer.cc \
		SimpleHTTPServer.h SimpleHTTPServer.cc \
		SimpleHTTPConn.h SimpleHTTPConn.cc
libhelen_server_la_CXXFLAGS = -I.. $(AM_CXXFLAGS)
libhelen_server_la_LDFLAGS = $(AM_LDFLAGS)
