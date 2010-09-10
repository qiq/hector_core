# current directory is writable, directory with this script may be not (VPATH builds -- make distcheck)

function doinit {
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
	export LD_LIBRARY_PATH=$base/lib:$base/lib/processing_engine/servers:$base/lib/processing_engine/modules:$base/lib/processing_engine/resources:$base/lib/perl/.libs:$LD_LIBRARY_PATH
	export PERL5LIB=$base/lib/perl:$base/lib/processing_engine/modules/perl:$PERL5LIB

	rm -f test.log
}

function doexit {
        if [ "$clean" = 1 ]; then
                rm -f $base/test.log.props
		clean=0
        fi
	if [ $0 != '-bash' ]; then
		exit 1;
	fi
}

function server_start {
        if ln -s $base/test/test.log.props $base/test.log.props 2>/dev/null; then
                clean=1
        fi
	if [ "$USE_VALGRIND" == 1 ]; then
		( cd $base; libtool --mode=execute valgrind --tool=memcheck --track-origins=yes --leak-check=full --leak-resolution=high --num-callers=20 --trace-children=yes --log-file=$base/test/${id}.log.valgrind server -c $base/test/${id}-config.xml -f test & )
		client_wait_dontfail robot_processing_engine.run 0
	else
		if ! ( cd $base; server -c ${base}/test/${id}-config.xml test ); then
			cat test.log 2>/dev/null
			doexit
		fi
	fi
}

function compare_result {
	diff -u $base/test/$id.log.correct $id.log.test
}
