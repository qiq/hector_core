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
	export LD_LIBRARY_PATH=$base/lib/.libs:$LD_LIBRARY_PATH

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
		( cd $base; libtool --mode=execute valgrind --leak-check=full --trace-children=yes server -c $base/test/$id-config.xml -f test 2>${id}.log.valgrind & )
		sleep 15;
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

function client_wait {
	x=`client_get $1`;
	while [ -n "$x" -a "$x" != "$2" ]; do
		bash -c "$SLEEP"
		x=`client_get $1`;
		echo "sleeping"
	done
}

function compare_result {
	diff -u $base/test/$id.log.correct $id.log.test
}
