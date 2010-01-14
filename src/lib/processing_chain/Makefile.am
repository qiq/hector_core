noinst_LTLIBRARIES = libhector_processing_chain.la

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

libhector_processing_chain_la_SOURCES = \
	ProcessingChain.h ProcessingChain.cc \
	Processor.h Processor.cc \
	Module.h Module.cc \
	OutputFilter.h
libhector_processing_chain_la_CXXFLAGS = -I.. -I../resources $(AM_CXXFLAGS)
libhector_processing_chain_la_LDFLAGS = $(AM_LDFLAGS)
