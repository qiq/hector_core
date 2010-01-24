AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ -I@PERL_INCLUDE_DIR@ -I.. -I../../lib/processing_chain
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ -module -avoid-version
LIBS = ../libhector_processing_chain.la
AM_LIBTOOLFLAGS = --tag=disable-static

BUILT_SOURCES =

if WITH_SWIG
if WITH_PERL

perldir = $(pkglibdir)/perl

BUILT_SOURCES += RPCMessage_wrap.cc RPCMessage.pm
RPCMessage_wrap.cc RPCMessage.pm: ../RPCMessage.i
	$(SWIG) -c++ -perl -o $@ $<

perl_SCRIPTS = perl_module.pl RPCMessage.pm
perl_LTLIBRARIES = RPCMessage.la
RPCMessage_la_SOURCES = \
	RPCMessage_wrap.cc

endif
endif

CLEANFILES = $(BUILT_SOURCES)
