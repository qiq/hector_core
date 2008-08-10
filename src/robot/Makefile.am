SUBDIRS = . modules

#AM_LDADD = -Wall
AM_CXXFLAGS = -Wall

bin_PROGRAMS = robot

#nobase_include_HEADERS = \
#	Resource.h \
#	ResourceQueue.h

robot_SOURCES = \
	Robot.h Robot.cc \
	Resource.h Resource.cc \
	Resource.pb.h Resource.pb.cc \
	ResourceQueue.h ResourceQueue.cc \
	RobotHTTPServer.h RobotHTTPServer.cc \
	ProcessingChain.h ProcessingChain.cc \
	Processor.h Processor.cc \
	ProcessorInput.h ProcessorInput.cc \
	ProcessorSimple.h ProcessorSimple.cc \
	ProcessorParallel.h ProcessorParallel.cc \
	ProcessorOutput.h ProcessorOutput.cc \
	Module.h \
	ModuleInput.h \
	ModuleSimple.h \
	ModuleParallel.h \
	ModuleOutput.h \
	Resource.proto

robot_CXXFLAGS = -I ../lib -I /usr/local/lib
robot_LDADD = -lpthread -L../lib -lhector -lltdl -lprotobuf -llog4cxx

Resource.pb.cc Resource.pb.h: Resource.proto
	/usr/local/bin/protoc Resource.proto --cpp_out=.

clean-local:
	rm -f Resource.pb.cc Resource.pb.h

test:
	echo "Test!"
