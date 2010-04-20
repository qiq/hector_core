AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ @JAVA_CFLAGS@ -I.. -I../processing_chain -I../../resources
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @JAVA_LDFLAGS@

BUILT_SOURCES =

if WITH_SWIG
if WITH_JAVA

BUILT_SOURCES += Hector_wrap.cc
Hector_wrap.cc: ../Hector.i
	$(SWIG) -c++ -java -o $@ $<

noinst_LTLIBRARIES = Hector.la
Hector_la_SOURCES = \
	Hector_wrap.cc
Hector_la_LIBADD = ../libhector.la
Hector_la_LDFLAGS = -module -avoid-version

endif
endif

clean-local:
	rm -f Hector_wrap.cc
