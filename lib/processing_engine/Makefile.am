SUBDIRS = resources modules servers

noinst_LTLIBRARIES = libhector_processing_engine.la

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

BUILT_SOURCES =

libhector_processing_engine_la_SOURCES = \
	ProcessingEngine.h ProcessingEngine.cc \
	Processor.h Processor.cc \
	Module.h Module.cc \
	OutputFilter.h \
	Resource.h \
	PerlModule.h

libhector_processing_engine_la_CXXFLAGS = -I.. $(AM_CXXFLAGS)
libhector_processing_engine_la_LIBADD = resources/libhector_resources.la
libhector_processing_engine_la_LDFLAGS = $(AM_LDFLAGS)

if WITH_SWIG
if WITH_PERL
libhector_processing_engine_la_SOURCES += \
	PerlModule.cc perlxsi.c
libhector_processing_engine_la_CXXFLAGS += @PERL_CFLAGS@
libhector_processing_engine_la_CFLAGS = ${libhector_processing_engine_la_CXXFLAGS} @PERL_CFLAGS@
libhector_processing_engine_la_LDFLAGS += @PERL_LDFLAGS@
endif
endif


# SWIG interface
EXTRA_DIST = Resource.i

CLEANFILES = $(BUILT_SOURCES)
