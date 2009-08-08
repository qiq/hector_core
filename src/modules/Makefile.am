#AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ -I../lib -I../lib/processing_chain
#AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ -version-info 0:0:0 -L../lib -lhector

AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@

lib_LTLIBRARIES = libmodule-dummy.la libmodule-load-resource.la libmodule-save-resource.la

libmodule_dummy_la_SOURCES = \
		ModuleDummy.h ModuleDummy.cc
libmodule_dummy_la_CXXFLAGS = $(AM_CXXFLAGS) -I../lib -I../lib/processing_chain
libmodule_dummy_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS)

libmodule_load_resource_la_SOURCES = \
		ModuleLoadResource.h ModuleLoadResource.cc
libmodule_load_resource_la_CXXFLAGS = $(AM_CXXFLAGS) -I../lib -I../lib/processing_chain
libmodule_load_resource_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS)

libmodule_save_resource_la_SOURCES = \
		ModuleSaveResource.h ModuleSaveResource.cc
libmodule_save_resource_la_CXXFLAGS = $(AM_CXXFLAGS) -I../lib -I../lib/processing_chain
libmodule_save_resource_la_LDFLAGS = -version-info 0:0:0 $(AM_LDFLAGS)
