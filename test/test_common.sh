# Include script for test*.sh
# Current directory is writable, directory with this script may be not (VPATH
# builds -- make distcheck).

base=$BASH_SOURCE
base=`dirname "$base"`
base=`readlink -f "$base"`'/..'
test_base=`readlink -f "$base"`

export "PATH=$test_base/src:$PATH"
export "LD_LIBRARY_PATH=$test_base/lib/.libs:$test_base/lib/processing_engine/server/.libs:$test_base/lib/processing_engine/resource/.libs:$test_base/lib/processing_engine/module/.libs:$test_base/lib/perl/.libs:$test_base/lib/python/.libs:$LD_LIBRARY_PATH"
export "PERL5LIB=$test_base/lib/perl:$test_base/lib/processing_engine/module/perl:$test_base/lib/processing_engine/resource/perl:$PERL5LIB"
export "PYTHONPATH=$test_base/lib/python:$test_base/lib/python/.libs:$test_base/lib/processing_engine/module/python:$PYTHONPATH"
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
	if [ -z "$VALGRIND" ]; then
		hector_server_start "$test_base/test/${id}.xml" -B "$test_base" $server $@
	else
		libtool --mode=execute valgrind --tool=memcheck --track-origins=yes --leak-check=full --leak-resolution=high --show-reachable=yes --num-callers=40 --trace-children=yes --gen-suppressions=all --suppressions=../hector_core.supp --log-file=$id.log.valgrind `which hector_server` -c "$test_base/test/${id}.xml" -B "$test_base" -f $server $@ &
	fi
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
	if [ -z "$VALGRIND" ]; then
		hector_server_start "$test_base/test/${id}.xml" -B "$test_base" -f -b $server $@
	else
		libtool --mode=execute valgrind --tool=memcheck --track-origins=yes --leak-check=full --leak-resolution=high --show-reachable=yes --num-callers=40 --trace-children=yes --gen-suppressions=all --suppressions=../hector_core.supp --log-file=$id.log.valgrind `which hector_server` -c "$test_base/test/${id}.xml" -B "$test_base" -f -b $server $@
	fi
}

function test_compare_result {
	id=$1
	if [ -z "$id" ]; then
		echo "usage: test_compare_result id"
		return
	fi
	diff -u "$test_base/test/${id}.log.correct" "${id}.log.result"
	result=$?
	if [ -n "$VALGRIND" ]; then
		definitely=`grep "definitely lost:" ${id}.log.valgrind | sed -e 's/.*definitely lost: \([0-9,]\+\).*/\1/'|sed -e 's/,//g'`
		indirectly=`grep "indirectly lost:" ${id}.log.valgrind | sed -e 's/.*indirectly lost: \([0-9,]\+\).*/\1/'|sed -e 's/,//g'`
		possibly=`grep "possibly lost:" ${id}.log.valgrind | sed -e 's/.*possibly lost: \([0-9,]\+\).*/\1/'|sed -e 's/,//g'`
		reachable=`grep "still reachable:" ${id}.log.valgrind | sed -e 's/.*still reachable: \([0-9,]\+\).*/\1/'|sed -e 's/,//g'`
		if [ "$definitely" != "0" -o "$indirectly" != "0" -o "$possibly" != "0" -o "$reachable" != "0" ]; then
			echo "memleaks: $definitely $indirectly $possibly $reachable"
			result=1;
		fi
	fi
	exit $result
}
