# Include script for test*.sh
# Current directory is writable, directory with this script may be not (VPATH
# builds -- make distcheck).

function test_init {
	if [ $0 == '-bash' -o $0 == 'bash' ]; then
		if [ -z "$id" ]; then
			echo "No id specified"
			return
		fi
		base=.
	else
		base=`dirname "$0"`
		id=`basename "$0" .sh`
	fi
	base=`readlink -f "$base"`'/..'
	base=`readlink -f "$base"`
	export PATH=$base/src:$PATH
	export LD_LIBRARY_PATH=$base/lib:$base/lib/processing_engine/servers:$base/lib/processing_engine/resources:$base/lib/processing_engine/modules:$base/lib/perl/.libs:$base/lib/python/.libs:$LD_LIBRARY_PATH
	export PERL5LIB=$base/lib/perl:$base/lib/processing_engine/modules/perl:$base/lib/processing_engine/resources/perl:$PERL5LIB
	export PYTHONPATH=$base/lib/python:$base/lib/python/.libs:$base/lib/processing_engine/modules/python:$PYTHONPATH
	HECTOR_HOST=localhost:1101

	# hector helper functions
	. hector_common.sh

	rm -f test.log
}

function test_finish {
	if [ -L test.log.props ]; then
		rm test.log.props;
	fi
}

function test_server_start {
	ln -s $base/test/test.log.props . 2>/dev/null
	hector_server_start $base/test/${id}_config.xml test $@
	hector_client_wait_dontfail PE_test.run 0
}

function test_server_batch {
	ln -s $base/test/test.log.props . 2>/dev/null
	hector_server_start $base/test/${id}_config.xml -f -b test $@
}

function test_compare_result {
	diff -u $base/test/$id.log.correct $id.log.test
}
