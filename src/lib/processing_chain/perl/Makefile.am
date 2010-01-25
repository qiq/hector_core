AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ -I@PERL_INCLUDE_DIR@ -I.. -I../..
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@ -module -avoid-version
LIBS = ../libhector_processing_chain.la ../../../resources/libhector_resources.la
AM_LIBTOOLFLAGS = --tag=disable-static

BUILT_SOURCES =

if WITH_SWIG
if WITH_PERL

perldir = $(pkglibdir)/perl

perl_SCRIPTS = perl_module.pl RPC.pm RPCMessage.pm
perl_LTLIBRARIES = RPC.la RPCMessage.la

BUILT_SOURCES += RPC_wrap.cc RPC.pm
RPC_wrap.cc RPC.pm: ../RPC.i
	$(SWIG) -c++ -perl -o $@ $<

RPC_la_SOURCES = \
	RPC_wrap.cc

BUILT_SOURCES += RPCMessage_wrap.cc RPCMessage.pm
RPCMessage_wrap.cc RPCMessage.pm: ../RPCMessage.i
	$(SWIG) -c++ -perl -o $@ $<

RPCMessage_la_SOURCES = \
	RPCMessage_wrap.cc

endif
endif

CLEANFILES = $(BUILT_SOURCES)
