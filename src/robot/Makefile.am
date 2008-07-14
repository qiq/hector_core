bin_PROGRAMS = robot

#nobase_include_HEADERS = \
#	Resource.h \
#	ResourceQueue.h

robot_SOURCES = \
	Robot.cc \
	Resource.cc \
	Resource.h \
	ResourceQueue.cc \
	ResourceQueue.h \
	RobotHTTPServer.cc \
	RobotHTTPServer.h

robot_CXXFLAGS = -I ../lib
robot_LDADD = -lpthread -L../lib -lwspy

test:
	echo "Test!"
