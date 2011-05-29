# Include script for test*.sh
# Current directory is writable, directory with this script may be not (VPATH
# builds -- make distcheck).

base=$BASH_SOURCE
base=`dirname "$base"`
base=`readlink -f "$base"`'/..'
test_base=`readlink -f "$base"`

export "PATH=$test_base/src:$PATH"
export "LD_LIBRARY_PATH=$test_base/lib/.libs:$test_base/lib/processing_engine/servers/.libs:$test_base/lib/processing_engine/resources/.libs:$test_base/lib/processing_engine/modules/.libs:$test_base/lib/perl/.libs:$test_base/lib/python/.libs:$LD_LIBRARY_PATH"
export "PERL5LIB=$test_base/lib/perl:$test_base/lib/processing_engine/modules/perl:$test_base/lib/processing_engine/resources/perl:$PERL5LIB"
export "PYTHONPATH=$test_base/lib/python:$test_base/lib/python/.libs:$test_base/lib/processing_engine/modules/python:$PYTHONPATH"
HECTOR_HOST=localhost:1101

# hector helper functions
. hector_common.sh

function test_server_start {
	id=$1
	server=$2
	if [ -z "$id" -o -z "$server" ]; then
		echo "usage: test_server_start id serverId [args]"
		return
	fi
	shift; shift
	hector_server_shutdown 2>/dev/null
	hector_server_start "$test_base/test/${id}.xml" -B "$test_base" $server $@
	hector_client_wait_dontfail PE_test.run 0
}

function test_server_shutdown {
	hector_server_shutdown
}

function test_server_batch {
	id=$1
	server=$2
	if [ -z "$id" -o -z "$server" ]; then
		echo "usage: test_server_batch id serverId"
		return
	fi
	shift; shift
	hector_server_start "$test_base/test/${id}.xml" -B "$test_base" -f -b $server $@
}

function test_compare_result {
	id=$1
	if [ -z "$id" ]; then
		echo "usage: test_compare_result id"
		return
	fi
	diff -u "$test_base/test/${id}.log.correct" "${id}.log.result"
}
