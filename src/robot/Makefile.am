SUBDIRS = . modules

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
	RobotHTTPServer.h \
	ProcessingChain.h \
	ProcessingChain.cc \
	Processor.h \
	Processor.cc \
	ProcessorInput.h \
	ProcessorInput.cc \
	ProcessorSimple.h \
	ProcessorSimple.cc \
	ProcessorParallel.h \
	ProcessorParallel.cc \
	ProcessorOutput.h \
	ProcessorOutput.cc \
	Module.h \
	ModuleSimple.h
	ModuleParallel.h

robot_CXXFLAGS = -I ../lib
robot_LDADD = -lpthread -L../lib -lwspy -lltdl

test:
	echo "Test!"
