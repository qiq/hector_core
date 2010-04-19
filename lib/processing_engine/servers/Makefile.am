AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ -module -avoid-version
AM_LIBTOOLFLAGS = --tag=disable-static

hectordir = $(pkglibdir)
hector_LTLIBRARIES = Robot.la

Robot_la_SOURCES = RobotHTTPServer.h RobotHTTPServer.cc
Robot_la_CXXFLAGS = $(AM_CXXFLAGS) -I ../../lib
Robot_la_LDFLAGS = $(AM_LDFLAGS)
