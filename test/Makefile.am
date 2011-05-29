TESTS_ENVIRONMENT = PATH='$(top_srcdir)/test:$(top_builddir)/src:$(PATH)' LD_LIBRARY_PATH=$(top_builddir)/lib/processing_engine/modules:$(top_builddir)/lib/processing_engine/resources:$(top_builddir)/lib/processing_engine/servers:$(top_builddir)/lib/perl/.libs:$(top_builddir)/lib/python/.libs:$(LD_LIBRARY_PATH) PERL5LIB=$(top_builddir)/lib/perl PYTHONPATH=$(top_builddir)/lib/python:$(top_builddir)/lib/python/.libs $(SHELL)

TESTS =
EXTRA_DIST = test_common.sh
CLEANFILES = test*.log.valgrind

TESTS += input_output.sh
EXTRA_DIST += input_output.sh input_output.xml input_output.log.correct
CLEANFILES += input_output.log input_output.log.result

TESTS += input_output_mt.sh
EXTRA_DIST += input_output_mt.sh input_output_mt.xml input_output_mt.log.correct
CLEANFILES += input_output_mt.log input_output_mt.log.result

TESTS += input_simple_output.sh
EXTRA_DIST += input_simple_output.sh input_simple_output.xml input_simple_output.log.correct
CLEANFILES += input_simple_output.log input_simple_output.log.result

TESTS += input_simple_output_mt.sh
EXTRA_DIST += input_simple_output_mt.sh input_simple_output_mt.xml input_simple_output_mt.log.correct
CLEANFILES += input_simple_output_mt.log input_simple_output_mt.log.result

TESTS += inputs_simple_output_mt.sh
EXTRA_DIST += inputs_simple_output_mt.sh inputs_simple_output_mt.xml inputs_simple_output_mt.log.correct
CLEANFILES += inputs_simple_output_mt.log inputs_simple_output_mt.log.result

TESTS += inputs_simple_outputs.sh
EXTRA_DIST += inputs_simple_outputs.sh inputs_simple_outputs.xml inputs_simple_outputs.log.correct
CLEANFILES += inputs_simple_outputs.log inputs_simple_outputs.log.result

TESTS += input_copy_output.sh
EXTRA_DIST += input_copy_output.sh input_copy_output.xml input_copy_output.log.correct
CLEANFILES += input_copy_output.log input_copy_output.log.result

TESTS += input_loop_output.sh
EXTRA_DIST += input_loop_output.sh input_loop_output.xml input_loop_output.log.correct
CLEANFILES += input_loop_output.log input_loop_output.log.result

TESTS += input_multi_output.sh
EXTRA_DIST += input_multi_output.sh input_multi_output.xml input_multi_output.log.correct
CLEANFILES += input_multi_output.log input_multi_output.log.result

TESTS += load_save.sh
EXTRA_DIST += load_save.sh load_save.xml load_save.log.correct load_save.data.in
CLEANFILES += load_save.log load_save.log.result load_save.data.out

TESTS += input_output_pl.sh
EXTRA_DIST += input_output_pl.sh input_output_pl.xml input_output_pl.log.correct
CLEANFILES += input_output_pl.log input_output_pl.log.result

TESTS += input_output_mt_pl.sh
EXTRA_DIST += input_output_mt_pl.sh input_output_mt_pl.xml input_output_mt_pl.log.correct
CLEANFILES += input_output_mt_pl.log input_output_mt_pl.log.result

TESTS += input_simple_output_pl.sh
EXTRA_DIST += input_simple_output_pl.sh input_simple_output_pl.xml input_simple_output_pl.log.correct
CLEANFILES += input_simple_output_pl.log input_simple_output_pl.log.result

TESTS += input_simple_output_mt_pl.sh
EXTRA_DIST += input_simple_output_mt_pl.sh input_simple_output_mt_pl.xml input_simple_output_mt_pl.log.correct
CLEANFILES += input_simple_output_mt_pl.log input_simple_output_mt_pl.log.result

TESTS += inputs_simple_output_mt_pl.sh
EXTRA_DIST += inputs_simple_output_mt_pl.sh inputs_simple_output_mt_pl.xml inputs_simple_output_mt_pl.log.correct
CLEANFILES += inputs_simple_output_mt_pl.log inputs_simple_output_mt_pl.log.result

TESTS += inputs_simple_outputs_pl.sh
EXTRA_DIST += inputs_simple_outputs_pl.sh inputs_simple_outputs_pl.xml inputs_simple_outputs_pl.log.correct
CLEANFILES += inputs_simple_outputs_pl.log inputs_simple_outputs_pl.log.result

TESTS += input_copy_output_pl.sh
EXTRA_DIST += input_copy_output_pl.sh input_copy_output_pl.xml input_copy_output_pl.log.correct
CLEANFILES += input_copy_output_pl.log input_copy_output_pl.log.result

TESTS += input_loop_output_pl.sh
EXTRA_DIST += input_loop_output_pl.sh input_loop_output_pl.xml input_loop_output_pl.log.correct
CLEANFILES += input_loop_output_pl.log input_loop_output_pl.log.result

TESTS += input_multi_output_pl.sh
EXTRA_DIST += input_multi_output_pl.sh input_multi_output_pl.xml input_multi_output_pl.log.correct
CLEANFILES += input_multi_output_pl.log input_multi_output_pl.log.result

TESTS += input_output_py.sh
EXTRA_DIST += input_output_py.sh input_output_py.xml input_output_py.log.correct
CLEANFILES += input_output_py.log input_output_py.log.result

TESTS += input_output_mt_py.sh
EXTRA_DIST += input_output_mt_py.sh input_output_mt_py.xml input_output_mt_py.log.correct
CLEANFILES += input_output_mt_py.log input_output_mt_py.log.result

TESTS += input_simple_output_py.sh
EXTRA_DIST += input_simple_output_py.sh input_simple_output_py.xml input_simple_output_py.log.correct
CLEANFILES += input_simple_output_py.log input_simple_output_py.log.result

TESTS += input_simple_output_mt_py.sh
EXTRA_DIST += input_simple_output_mt_py.sh input_simple_output_mt_py.xml input_simple_output_mt_py.log.correct
CLEANFILES += input_simple_output_mt_py.log input_simple_output_mt_py.log.result

TESTS += inputs_simple_output_mt_py.sh
EXTRA_DIST += inputs_simple_output_mt_py.sh inputs_simple_output_mt_py.xml inputs_simple_output_mt_py.log.correct
CLEANFILES += inputs_simple_output_mt_py.log inputs_simple_output_mt_py.log.result

TESTS += inputs_simple_outputs_py.sh
EXTRA_DIST += inputs_simple_outputs_py.sh inputs_simple_outputs_py.xml inputs_simple_outputs_py.log.correct
CLEANFILES += inputs_simple_outputs_py.log inputs_simple_outputs_py.log.result

TESTS += input_copy_output_py.sh
EXTRA_DIST += input_copy_output_py.sh input_copy_output_py.xml input_copy_output_py.log.correct
CLEANFILES += input_copy_output_py.log input_copy_output_py.log.result

TESTS += input_loop_output_py.sh
EXTRA_DIST += input_loop_output_py.sh input_loop_output_py.xml input_loop_output_py.log.correct
CLEANFILES += input_loop_output_py.log input_loop_output_py.log.result

TESTS += input_multi_output_py.sh
EXTRA_DIST += input_multi_output_py.sh input_multi_output_py.xml input_multi_output_py.log.correct
CLEANFILES += input_multi_output_py.log input_multi_output_py.log.result

TESTS += load_save2.sh
EXTRA_DIST += load_save2.sh load_save2.xml load_save2.log.correct load_save2.data.in
CLEANFILES += load_save2.log load_save2.log.result load_save2.data.out

TESTS += process.sh
EXTRA_DIST += process.sh process.xml process.log.correct process.data.in
CLEANFILES += process.log process.log.result process.data.out

TESTS += call.sh
EXTRA_DIST += call.sh call.xml call.log.correct
CLEANFILES += call.log call.log.result

TESTS += input_output_perl_resource.sh
EXTRA_DIST += input_output_perl_resource.sh input_output_perl_resource.xml input_output_perl_resource.log.correct
CLEANFILES += input_output_perl_resource.log input_output_perl_resource.log.result

TESTS += input_output_perl_resource_pl.sh
EXTRA_DIST += input_output_perl_resource_pl.sh input_output_perl_resource_pl.xml input_output_perl_resource_pl.log.correct
CLEANFILES += input_output_perl_resource_pl.log input_output_perl_resource_pl.log.result
