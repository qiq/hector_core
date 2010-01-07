
AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

lib_LTLIBRARIES = libserver_robot.la

libserver_robot_la_SOURCES = RobotHTTPServer.h RobotHTTPServer.cc
libserver_robot_la_CXXFLAGS = $(AM_CXXFLAGS) -I ../../lib
libserver_robot_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS)
