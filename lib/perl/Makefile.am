AM_CXXFLAGS = @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ @PERL_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resources
AM_LDFLAGS = @LOG4CXX_LIBS@ @PROTOBUF_LIBS@ @PERL_LDFLAGS@ -module -avoid-version
AM_LIBTOOLFLAGS = --tag=disable-static

BUILT_SOURCES =

if WITH_SWIG
if WITH_PERL

perldir = $(libdir)/hector/modules/perl
perl_SCRIPTS = Hector.pm
perl_LTLIBRARIES = Hector.la

BUILT_SOURCES += Hector_wrap.cc Hector.pm
Hector_wrap.cc Hector.pm: $(top_srcdir)/lib/Hector.i
	$(SWIG) -c++ -perl -o Hector_wrap.cc $<

Hector_la_SOURCES = \
	Hector_wrap.cc
Hector_la_LIBADD = $(top_builddir)/lib/libhector.la

endif
endif

CLEANFILES = $(BUILT_SOURCES)
