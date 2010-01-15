AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

BUILT_SOURCES =

if WITH_SWIG
if WITH_JAVA

BUILT_SOURCES += WebResource_wrap.cc
WebResource_wrap.cc: ../WebResource.i
	$(SWIG) -c++ -java -o $@ $<

lib_LTLIBRARIES = WebResource.la
WebResource_la_SOURCES = \
	WebResource_wrap.cc
WebResource_la_LIBADD = ../libhector_resources.la
WebResource_la_LDFLAGS = -module -avoid-version

endif
endif

clean-local:
	rm -f WebResource_wrap.cc
