SUBDIRS = server processing_chain perl

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@

lib_LTLIBRARIES = libhector.la

libhector_la_SOURCES = \
		common.h common.cc \
		Lock.h CondLock.h \
		Config.h Config.cc \
		CyclicBuffer.h \
		SyncQueue.h \
		LibraryLoader.h LibraryLoader.cc \
		Object.h ObjectRegistry.h ObjectRegistry.cc \
		Server.h Server.cc \
		Connection.h ProcessConnection.h ProcessConnection.cc StdioConnection.h StdioConnection.cc
libhector_la_CXXFLAGS = $(AM_CXXFLAGS)
libhector_la_LIBADD = server/libhector_server.la processing_chain/libhector_processing_chain.la ../resources/libhector_resources.la
libhector_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS)

# SWIG interface
EXTRA_DIST = Connection.i StdioConnection.i
