SUBDIRS = server processing_engine . perl java python

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@

lib_LTLIBRARIES = libhelen.la

libhelen_la_SOURCES = \
		common.h common.cc \
		Lock.h CondLock.h \
		Config.h Config.cc \
		CyclicBuffer.h \
		SyncQueue.h \
		LibraryLoader.h LibraryLoader.cc \
		Object.h Object.cc ObjectRegistry.h ObjectRegistry.cc ObjectValues.h \
		Server.h Server.cc \
		Connection.h ProcessConnection.h ProcessConnection.cc StdioConnection.h StdioConnection.cc
libhelen_la_CXXFLAGS = $(AM_CXXFLAGS)
libhelen_la_LIBADD = server/libhelen_server.la processing_engine/libhelen_processing_engine.la
libhelen_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS) -lltdl

# SWIG interface
EXTRA_DIST = Connection.i StdioConnection.i Hector.i
