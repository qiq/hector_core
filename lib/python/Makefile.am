AM_CXXFLAGS = @LOG4CXX_CFLAGS@ @PYTHON_CFLAGS@ @PROTOBUF_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resources
AM_LDFLAGS = @LOG4CXX_LIBS@ @PYTHON_LDFLAGS@ @PROTOBUF_LIBS@ -module -avoid-version
AM_LIBTOOLFLAGS = --tag=disable-static

BUILT_SOURCES =

if WITH_SWIG
if WITH_PYTHON

pythondir = $(libdir)/hector/modules/python
python_SCRIPTS = Hector.py
python_LTLIBRARIES = _Hector.la

BUILT_SOURCES += Hector_wrap.cc Hector.py
Hector_wrap.cc Hector.py: $(top_srcdir)/lib/Hector.i
	$(SWIG) -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resources -c++ -python -o $@ $<

_Hector_la_SOURCES = \
	Hector_wrap.cc
_Hector_la_LIBADD = $(top_builddir)/lib/libhector.la

endif
endif

CLEANFILES = $(BUILT_SOURCES) *.pyc
