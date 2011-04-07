SUBDIRS = server processing_engine . perl java python

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/server
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

lib_LTLIBRARIES = libhector.la

hectorincludedir = $(includedir)/hector
hectorinclude_HEADERS = common.h Connection.h StdioConnection.h PlainLock.h RWLock.h CondLock.h MemoryPool.h \
	IpAddr.h IpAddr.i LibraryLoader.h SyncQueue.h Object.h ObjectRegistry.h ObjectValues.h \
	Hector.i Object.i ObjectRegistry.i Connection.i StdioConnection.i MD5.h

libhector_la_SOURCES = \
		common.h common.cc \
		MD5.h MD5.cc \
		PlainLock.h RWLock.h CondLock.h MemoryPool.h \
		IpAddr.h IpAddr.cc \
		Config.h Config.cc \
		CyclicBuffer.h \
		SyncQueue.h \
		LibraryLoader.h LibraryLoader.cc \
		Object.h Object.cc ObjectRegistry.h ObjectRegistry.cc ObjectValues.h \
		Connection.h ProcessConnection.h ProcessConnection.cc StdioConnection.h StdioConnection.cc
libhector_la_CXXFLAGS = $(AM_CXXFLAGS)
libhector_la_LIBADD = server/libhector_server.la processing_engine/libhector_processing_engine.la -lltdl
libhector_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS)

if WITH_SWIG
if WITH_PERL
libhector_la_SOURCES += \
	EmbeddedPerl.h EmbeddedPerl.cc perlxsi.c
libhector_la_CXXFLAGS += @PERL_CFLAGS@
libhector_la_CFLAGS = ${libhector_la_CXXFLAGS}
libhector_la_LDFLAGS += @PERL_LDFLAGS@
endif

if WITH_PYTHON
libhector_la_SOURCES += \
	EmbeddedPython.h EmbeddedPython.cc
libhector_la_CXXFLAGS += @PYTHON_CFLAGS@
libhector_la_LDFLAGS += @PYTHON_LDFLAGS@
endif
endif

# SWIG interface
EXTRA_DIST = Connection.i StdioConnection.i Hector.i Object.i ObjectRegistry.i
