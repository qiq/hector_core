AM_CXXFLAGS = @LOG4CXX_CFLAGS@ @PERL_CFLAGS@ @PROTOBUF_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resource
AM_LDFLAGS = @LOG4CXX_LIBS@ @PERL_LDFLAGS@ @PROTOBUF_LIBS@ -module -avoid-version
AM_LIBTOOLFLAGS = --tag=disable-static

BUILT_SOURCES =

if WITH_SWIG
if WITH_PERL

perldir = $(libdir)/hector/module/perl
perl_SCRIPTS = Hector.pm
perl_LTLIBRARIES = Hector.la

BUILT_SOURCES += Hector_wrap.cc Hector.pm
Hector_wrap.cc Hector.pm: $(top_srcdir)/lib/Hector.i
	$(SWIG) -Wall -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resource -DSWIG_TYPE_TABLE=hector -c++ -perl -o Hector_wrap.cc $<

Hector_la_SOURCES = \
	Hector_wrap.cc
Hector_la_LIBADD = $(top_builddir)/lib/libhector.la $(top_builddir)/lib/processing_engine/resource/libTestResource.la $(top_builddir)/lib/processing_engine/resource/libTestProtobufResource.la $(top_builddir)/lib/processing_engine/resource/libMarkerResource.la 


endif
endif

CLEANFILES = $(BUILT_SOURCES)
