AM_CXXFLAGS = @LOG4CXX_CFLAGS@ -I$(top_srcdir)/lib -I$(top_srcdir)/lib/processing_engine
AM_LDFLAGS = @LOG4CXX_LIBS@
AM_LIBTOOLFLAGS = --tag=disable-static

EXTRA_DIST =
BUILT_SOURCES = 
CLEANFILES =
ALIASES =

resourcesdir = $(libdir)/hector/resources
resources_LTLIBRARIES = libTestResource.la TestResource.la libTestProtobufResource.la TestProtobufResource.la

hectorincludedir = $(includedir)/hector
hectorinclude_HEADERS = TestResource.h TestProtobufResource.h TestProtobufResource.pb.h \
	TestResource.i TestProtobufResource.i

libTestResource_la_SOURCES = TestResource.h TestResource.cc
libTestResource_la_LDFLAGS = $(AM_LDFLAGS)

TestResource_la_SOURCES = TestResourceWrapper.h TestResourceWrapper.cc
TestResource_la_LDFLAGS = $(AM_LDFLAGS) -module -avoid-version
TestResource_la_LIBADD = libTestResource.la

EXTRA_DIST += TestResource.i
ALIASES += TestResource.la

libTestProtobufResource_la_SOURCES = TestProtobufResource.h TestProtobufResource.cc TestProtobufResource.pb.h TestProtobufResource.pb.cc
libTestProtobufResource_la_CXXFLAGS = $(AM_CXXFLAGS) @PROTOBUF_CFLAGS@
libTestProtobufResource_la_LDFLAGS = $(AM_LDFLAGS) @PROTOBUF_LIBS@

BUILT_SOURCES += TestProtobufResource.pb.h TestProtobufResource.pb.cc
TestProtobfuResource.pb.cc TestProtobufResource.pb.h: TestProtobufResource.proto
	$(PROTOC) TestProtobufResource.proto --cpp_out=.

TestProtobufResource_la_SOURCES = TestProtobufResourceWrapper.h TestProtobufResourceWrapper.cc
TestProtobufResource_la_CXXFLAGS = $(AM_CXXFLAGS) @PROTOBUF_CFLAGS@
TestProtobufResource_la_LDFLAGS = $(AM_LDFLAGS) -module -avoid-version @PROTOBUF_LIBS@
TestProtobufResource_la_LIBADD = libTestProtobufResource.la

EXTRA_DIST += TestProtobufResource.i TestProtobufResource.proto
CLEANFILES += TestProtobufResource.pb.h TestProtobufResource.pb.cc
ALIASES += TestProtobufResource.la

# In order to enable Resources::AcquireResource(id) to work, we need to know
# mapping id -> library_name. This is achieved using symlink id.la ->
# resource.la, so the same library is opened.

all-local: $(ALIASES)
	for i in $(ALIASES); do \
		h=`echo $$i|sed -e 's/\.la/\.h/'`; \
		id=`grep "int typeId" $(top_srcdir)/lib/processing_engine/resources/$$h|sed -e 's/.*int typeId = \([0-9]\+\).*/\1/'`; \
		ln -fs $$i $${id}.la; \
	done

clean-local:
	rm -f {0,1,2,3,4,5,6,7,8,9}*.la

install-exec-local:
	$(INSTALL) -d $(DESTDIR)$(resourcesdir); \
	for i in $(ALIASES); do \
		h=`echo $$i|sed -e 's/\.la/\.h/'`; \
		id=`grep "int typeId" $(top_srcdir)/lib/processing_engine/resources/$$h|sed -e 's/.*int typeId = \([0-9]\+\).*/\1/'`; \
		ln -fs $$i $(DESTDIR)$(resourcesdir)/$${id}.la; \
	done
