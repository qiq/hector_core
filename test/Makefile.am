TESTS_ENVIRONMENT = PATH='$(top_srcdir)/test:$(top_builddir)/src:$(PATH)' LD_LIBRARY_PATH=$(top_builddir)/lib/processing_engine/modules:$(top_builddir)/lib/processing_engine/resources:$(top_builddir)/lib/processing_engine/servers:$(LD_LIBRARY_PATH) $(SHELL)

TESTS =
EXTRA_DIST = common.sh test.log.props
CLEANFILES = test.log test*.log.valgrind

TESTS += test1.sh
EXTRA_DIST += test1.sh test1-config.xml test1.log.correct
CLEANFILES += test1.log.test

TESTS += test2.sh
EXTRA_DIST += test2.sh test2-config.xml test2.log.correct
CLEANFILES += test2.log.test

TESTS += test3.sh
EXTRA_DIST += test3.sh test3-config.xml test3.log.correct
CLEANFILES += test3.log.test

TESTS += test4.sh
EXTRA_DIST += test4.sh test4-config.xml test4.log.correct
CLEANFILES += test4.log.test

TESTS += test5.sh
EXTRA_DIST += test5.sh test5-config.xml test5.log.correct
CLEANFILES += test5.log.test

TESTS += test6.sh
EXTRA_DIST += test6.sh test6-config.xml test6.log.correct
CLEANFILES += test6.log.test

TESTS += test7.sh
EXTRA_DIST += test7.sh test7-config.xml test7.log.correct
CLEANFILES += test7.log.test

TESTS += test8.sh
EXTRA_DIST += test8.sh test8-config.xml test8.log.correct
CLEANFILES += test8.log.test

TESTS += test9.sh
EXTRA_DIST += test9.sh test9-config.xml test9.log.correct
CLEANFILES += test9.log.test

TESTS += test10.sh
EXTRA_DIST += test10.sh test10-config.xml test10.log.correct
CLEANFILES += test10.log.test
