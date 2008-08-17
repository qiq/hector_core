lib_LTLIBRARIES = libhector.la

libhector_la_CXXFLAGS = -I/usr/include/libxml2
libhector_la_LDFLAGS = -lxml2 -llog4cxx -g

libhector_la_SOURCES = common.h common.cc \
		Lock.h \
		SimpleServer.h SimpleServer.cc \
		SimpleHTTPServer.h SimpleHTTPServer.cc \
		SimpleHTTPConn.h SimpleHTTPConn.cc \
		Config.h Config.cc \
		CyclicBuffer.h CyclicBuffer.cc \
		SyncQueue.h
