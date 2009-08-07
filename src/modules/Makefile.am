AM_CXXFLAGS = @LIBXML2_CFLAGS@ @LOG4CXX_CFLAGS@ -I../lib -I../lib/processing_chain
AM_LDFLAGS = @LIBXML2_LIBS@ @LOG4CXX_LIBS@ -version-info 0:0:0 -L../lib -lhector

lib_LTLIBRARIES = libmodule-dummy.la libmodule-load-resource.la libmodule-save-resource.la

libmodule_dummy_la_SOURCES = \
		ModuleDummy.h ModuleDummy.cc

libmodule_load_resource_la_SOURCES = \
		ModuleLoadResource.h ModuleLoadResource.cc

libmodule_save_resource_la_SOURCES = \
		ModuleLoadResource.h ModuleLoadResource.cc
