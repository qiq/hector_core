SUBDIRS = resources modules servers

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ -I$(top_srcdir)/lib
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

noinst_LTLIBRARIES = libhector_processing_engine.la

hectorincludedir = $(includedir)/hector
hectorinclude_HEADERS = ProcessingEngine.h Processor.h Module.h Resource.h ProtobufResource.h \
	Resource.i ProtobufResource.i Module.i CallProcessingEngineModule.h

BUILT_SOURCES =

libhector_processing_engine_la_SOURCES = \
	ProcessingEngine.h ProcessingEngine.cc \
	Processor.h Processor.cc \
	Module.h Module.cc \
	OutputFilter.h \
	Resource.h ProtobufResource.h ProtobufResource.cc \
	ResourceFieldInfo.h \
	Resources.h Resources.cc \
	PerlModule.h \
	CallProcessingEngineModule.h CallProcessingEngineModule.cc

libhector_processing_engine_la_CXXFLAGS = $(AM_CXXFLAGS)
libhector_processing_engine_la_LDFLAGS = $(AM_LDFLAGS)

if WITH_SWIG
if WITH_PERL
libhector_processing_engine_la_SOURCES += \
	PerlModule.cc perlxsi.c
libhector_processing_engine_la_CXXFLAGS += @PERL_CFLAGS@ @PROTOBUF_CFLAGS@
libhector_processing_engine_la_CFLAGS = ${libhector_processing_engine_la_CXXFLAGS} @PERL_CFLAGS@ @PROTOBUF_CFLAGS@
libhector_processing_engine_la_LDFLAGS += @PERL_LDFLAGS@ @PROTOBUF_LIBS@
endif
endif


# SWIG interface
EXTRA_DIST = Resource.i ProtobufResource.i Module.i

CLEANFILES = $(BUILT_SOURCES)
