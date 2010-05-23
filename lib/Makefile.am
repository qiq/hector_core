SUBDIRS = server processing_engine . perl java python

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/server
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

lib_LTLIBRARIES = libhector.la

hectorincludedir = $(includedir)/hector
hectorinclude_HEADERS = common.h Connection.h StdioConnection.h PlainLock.h RWLock.h CondLock.h LibraryLoader.h SyncQueue.h Object.h ObjectRegistry.h ObjectValues.h Server.h \
	Hector.i Object.i ObjectRegistry.i Connection.i StdioConnection.i

libhector_la_SOURCES = \
		common.h common.cc \
		PlainLock.h RWLock.h CondLock.h \
		Config.h Config.cc \
		CyclicBuffer.h \
		SyncQueue.h \
		LibraryLoader.h LibraryLoader.cc \
		Object.h Object.cc ObjectRegistry.h ObjectRegistry.cc ObjectValues.h \
		Server.h Server.cc \
		Connection.h ProcessConnection.h ProcessConnection.cc StdioConnection.h StdioConnection.cc
libhector_la_CXXFLAGS = $(AM_CXXFLAGS)
libhector_la_LIBADD = server/libhector_server.la processing_engine/libhector_processing_engine.la
libhector_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS) -lltdl

# SWIG interface
EXTRA_DIST = Connection.i StdioConnection.i Hector.i Object.i ObjectRegistry.i
