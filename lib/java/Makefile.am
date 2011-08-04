AM_CXXFLAGS = @LOG4CXX_CFLAGS@ @JAVA_CFLAGS@ @PERL_CFLAGS@ @PROTOBUF_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resource
AM_LDFLAGS = @LOG4CXX_LIBS@ @JAVA_LDFLAGS@ @PERL_LDFLAGS@ @PROTOBUF_LIBS@

BUILT_SOURCES =

if WITH_SWIG
if WITH_JAVA

javadir = $(libdir)/hector/module/java
java_LTLIBRARIES = Hector.la

BUILT_SOURCES += Hector_wrap.cc
Hector_wrap.cc: $(top_srcdir)/lib/Hector.i
	$(SWIG) -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resource -c++ -java -o $@ $<

Hector_la_SOURCES = \
	Hector_wrap.cc
Hector_la_LIBADD = $(top_builddir)/lib/libhector.la
Hector_la_LDFLAGS = -module -avoid-version

endif
endif

clean-local:
	rm -f Hector_wrap.cc *.java
