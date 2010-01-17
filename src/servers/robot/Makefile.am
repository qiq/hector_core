
AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ -module -avoid-version

serversdir = $(pkglibdir)/servers
servers_LTLIBRARIES = Robot.la

Robot_la_SOURCES = RobotHTTPServer.h RobotHTTPServer.cc
Robot_la_CXXFLAGS = $(AM_CXXFLAGS) -I ../../lib
Robot_la_LDFLAGS = $(AM_LDFLAGS)
