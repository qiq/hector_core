AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ @PERL_CFLAGS@ -I.. -I../processing_engine -I../processing_engine/resources
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@ @PERL_LDFLAGS@ -module -avoid-version
LIBS = ../libhector.la
AM_LIBTOOLFLAGS = --tag=disable-static

BUILT_SOURCES =

if WITH_SWIG
if WITH_PERL

perldir = $(pkglibdir)/modules/perl

perl_SCRIPTS = Hector.pm
perl_LTLIBRARIES = Hector.la

BUILT_SOURCES += Hector_wrap.cc Hector.pm
Hector_wrap.cc Hector.pm: ../Hector.i
	$(SWIG) -c++ -perl -o $@ $<

Hector_la_SOURCES = \
	Hector_wrap.cc

endif
endif

CLEANFILES = $(BUILT_SOURCES)
