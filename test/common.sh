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
	export LD_LIBRARY_PATH=$base/lib:$base/lib/processing_engine/servers:$base/lib/processing_engine/resources:$base/lib/processing_engine/modules:$base/lib/perl/.libs:$LD_LIBRARY_PATH
	export PERL5LIB=$base/lib/perl:$base/lib/processing_engine/modules/perl:$PERL5LIB

	# hector helper functions
	. hector_common.sh

	rm -f test.log
}

function test_finish {
	if [ -L test.log.props ]; then
		rm test.log.props;
	fi
#	if [ $0 != '-bash' ]; then
#		exit 1;
#	fi
}

function test_server_start {
	ln -s $base/test/test.log.props . 2>/dev/null
	if [ "$USE_VALGRIND" == 1 ]; then
		libtool --mode=execute valgrind --tool=memcheck --track-origins=yes --leak-check=full --leak-resolution=high --num-callers=20 --trace-children=yes --log-file=hector_server.log.valgrind $base/src/hector_server -c $base/test/${id}-config.xml test &
                hector_client_wait_dontfail test_processing_engine.run 0
        else
		hector_server_start $base/test/${id}-config.xml test
        fi


}

#function server_start {
#        if ln -s $base/test/test.log.props $base/test.log.props 2>/dev/null; then
#                clean=1
#        fi
#	if [ "$USE_VALGRIND" == 1 ]; then
#		( cd $base; libtool --mode=execute valgrind --tool=memcheck --track-origins=yes --leak-check=full --leak-resolution=high --num-callers=20 --trace-children=yes --log-file=$base/test/${id}.log.valgrind hector_server -c $base/test/${id}-config.xml -f test & )
#		client_wait_dontfail robot_processing_engine.run 0
#	else
#		if ! ( cd $base; hector_server -c ${base}/test/${id}-config.xml test ); then
#			cat test.log 2>/dev/null
#			doexit
#		fi
#	fi
#}

function test_compare_result {
	diff -u $base/test/$id.log.correct $id.log.test
}
