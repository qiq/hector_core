AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ @PROTOBUF_CFLAGS@ @PYTHON_CFLAGS@ -I.. -I../processing_engine -I../processing_engine/resources
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ @PROTOBUF_LIBS@ @PYTHON_LDFLAGS@ -module -avoid-version
LIBS = ../libhector.la
AM_LIBTOOLFLAGS = --tag=disable-static

BUILT_SOURCES =

if WITH_SWIG
if WITH_PYTHON

pythondir = $(libdir)/hector/modules/python
python_SCRIPTS = Hector.py
python_LTLIBRARIES = _Hector.la

BUILT_SOURCES += Hector_wrap.cc Hector.py
Hector_wrap.cc Hector.py: ../Hector.i
	$(SWIG) -c++ -python -o $@ $<

_Hector_la_SOURCES = \
	Hector_wrap.cc

endif
endif

CLEANFILES = $(BUILT_SOURCES) *.pyc
