AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ @JAVA_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resources
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @JAVA_LDFLAGS@

BUILT_SOURCES =

if WITH_SWIG
if WITH_JAVA

javadir = $(libdir)/hector/modules/java
java_LTLIBRARIES = Hector.la

BUILT_SOURCES += Hector_wrap.cc
Hector_wrap.cc: $(top_srcdir)/lib/Hector.i
	$(SWIG) -c++ -java -o $@ $<

Hector_la_SOURCES = \
	Hector_wrap.cc
Hector_la_LIBADD = $(top_builddir)/lib/libhector.la
Hector_la_LDFLAGS = -module -avoid-version

endif
endif

clean-local:
	rm -f Hector_wrap.cc
