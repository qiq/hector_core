lib_LTLIBRARIES = libwspy.la

libwspy_la_CXXFLAGS = -I/usr/include/libxml2
libwspy_la_LDFLAGS = -lxml2 -llog4cxx

libwspy_la_SOURCES = common.cc \
		SimpleServer.cc \
		SimpleHTTPServer.cc \
		SimpleHTTPConn.cc \
		Config.cc \
		CyclicBuffer.cc
