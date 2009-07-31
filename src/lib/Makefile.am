SUBDIRS = server processing_chain

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

lib_LTLIBRARIES = libhector.la
bin_PROGRAMS = robot

libhector_la_SOURCES = \
		common.h common.cc \
		Lock.h CondLock.h \
		Config.h Config.cc \
		CyclicBuffer.h CyclicBuffer.cc \
		SyncQueue.h
libhector_la_CXXFLAGS = $(AM_CXXFLAGS)
libhector_la_LIBADD = server/libhector_server.la
libhector_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS)

robot_SOURCES = \
	Robot.cc RobotHTTPServer.cc RobotHTTPServer.h
robot_LDFLAGS = -lhector
