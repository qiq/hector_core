AM_CXXFLAGS = @LOG4CXX_CFLAGS@ @PYTHON_CFLAGS@ @PERL_CFLAGS@ @PROTOBUF_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resources
AM_LDFLAGS = @LOG4CXX_LIBS@ @PYTHON_LDFLAGS@ @PERL_LDFLAGS@ @PROTOBUF_LIBS@ -module -avoid-version
AM_LIBTOOLFLAGS = --tag=disable-static

BUILT_SOURCES =

if WITH_SWIG
if WITH_PYTHON

pythondir = $(libdir)/hector/modules/python
python_SCRIPTS = Hector.py
python_LTLIBRARIES = _Hector.la

BUILT_SOURCES += Hector_wrap.cc Hector.py
Hector_wrap.cc Hector.py: $(top_srcdir)/lib/Hector.i
	$(SWIG) -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine -I$(top_srcdir)/lib/processing_engine/resources -DSWIG_TYPE_TABLE=hector -c++ -python -threads -o Hector_wrap.cc $<

_Hector_la_SOURCES = \
	Hector_wrap.cc
# set SWIGINTERN to emtpy string, so that SWIG functions are not declared static and we may call them
_Hector_la_CXXFLAGS = $(AM_CXXFLAGS) -DSWIGINTERN=
_Hector_la_LIBADD = $(top_builddir)/lib/libhector.la $(top_builddir)/lib/processing_engine/resources/libTestResource.la $(top_builddir)/lib/processing_engine/resources/libTestProtobufResource.la $(top_builddir)/lib/processing_engine/resources/libMarkerResource.la

endif
endif

CLEANFILES = $(BUILT_SOURCES) *.pyc
