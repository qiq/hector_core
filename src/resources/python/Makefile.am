AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ -I@PYTHON_INCLUDE_DIR@ -I.. -I../../lib/processing_chain
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@ -module -avoid-version
LIBS = ../libhector_resources.la
AM_LIBTOOLFLAGS = --tag=disable-static

BUILT_SOURCES =

if WITH_SWIG
if WITH_PYTHON

pythondir = $(pkglibdir)/python

BUILT_SOURCES += Resources_wrap.cc Resources.py
Resources_wrap.cc Resources.py: ../Resources.i
	$(SWIG) -c++ -python -o $@ $<

BUILT_SOURCES += WebResource_wrap.cc WebResource.py
WebResource_wrap.cc WebResource.py: ../WebResource.i
	$(SWIG) -c++ -python -o $@ $<

python_SCRIPTS = Resources.py WebResource.py
python_LTLIBRARIES = _Resources.la _WebResource.la
_WebResource_la_SOURCES = \
	WebResource_wrap.cc

_Resources_la_SOURCES = \
	Resources_wrap.cc

endif
endif

CLEANFILES = $(BUILT_SOURCES) *.pyc
