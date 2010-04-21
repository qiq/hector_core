AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ -I$(top_srcdir)/lib
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

noinst_LTLIBRARIES = libhector_server.la

hectorincludedir = $(includedir)/hector
hectorinclude_HEADERS = SimpleServer.h SimpleHTTPServer.h SimpleHTTPConn.h

libhector_server_la_SOURCES = \
		SimpleServer.h SimpleServer.cc \
		SimpleHTTPServer.h SimpleHTTPServer.cc \
		SimpleHTTPConn.h SimpleHTTPConn.cc
libhector_server_la_CXXFLAGS = $(AM_CXXFLAGS)
libhector_server_la_LDFLAGS = $(AM_LDFLAGS)
