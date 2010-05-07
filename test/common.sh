#!/bin/bash

# current directory is writable, directory with this script may be not (VPATH builds -- make distcheck)

function doinit {
	if which perl >/dev/null; then
		SLEEP="perl -e 'select(undef, undef, undef, 0.25)';"
	else
		SLEEP="sleep 1";
	fi

	if [ $0 == '-bash' ]; then
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
                rm -f test.log.props
		clean=0
        fi
	if [ $0 != '-bash' ]; then
		exit 1;
	fi
}

function server_start {
        if ln -s $base/test/test.log.props . 2>/dev/null; then
                clean=1
        fi
	if [ "$USE_VALGRIND" == 1 ]; then
		( cd $base; libtool --mode=execute valgrind --tool=memcheck --track-origins=yes --leak-check=full --leak-resolution=high --num-callers=20 --trace-children=yes --log-file=${id}.log.valgrind server -c $base/test/$id-config.xml -f test & )
		client_wait robot_processing_engine.run 0 1
	else
		if ! server -c $base/test/$id-config.xml test; then
			cat test.log 2>/dev/null
			doexit
		fi
	fi
}

function server_shutdown {
	echo "shutdown" | client || doexit
        if [ "$clean" = 1 ]; then
                rm -f test.log.props
        fi
}

function client_set {
	echo "set $1=$2" | client || doexit
}

function client_get {
	echo "get $1" | ( client || doexit ) | sed -e 's/.*= //'
}

function client_get_dontfail {
	echo "get $1" | client 2>/dev/null | sed -e 's/.*= //'
}

function client_wait {
	if [ -z "$3" ]; then
		x=`client_get $1`;
	else
		x=`client_get_dontfail $1`
	fi
	while [ -z "$x" -o "$x" != "$2" ]; do
		bash -c "$SLEEP"
		if [ -z "$3" ]; then
			x=`client_get $1`;
		else
			x=`client_get_dontfail $1`
		fi
		echo "sleeping"
	done
}

function compare_result {
	diff -u $base/test/$id.log.correct $id.log.test
}
