#!/bin/bash

# simple test: one reader, one writer, single threaded

if which perl >/dev/null; then
	SLEEP="perl -e 'select(undef, undef, undef, 0.25)';"
else
	SLEEP="sleep 1";
fi

function init {
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
	cd $base
	base=`readlink -f "$base"`'/../src'
	base=`readlink -f "$base"`

	export LD_LIBRARY_PATH=$base:$base/lib/.libs:$LD_LIBRARY_PATH

	rm -f test.log
}

function doexit {
	if [ $0 != '-bash' ]; then
		exit 1;
	fi
}

function server_start {
	if [ "$USE_VALGRIND" == 1 ]; then
		libtool --mode=execute valgrind --leak-check=full --trace-children=yes $base/bin/server -c $id-config.xml -f test 2>${id}.log.valgrind &
		sleep 15;
	else
		if ! $base/bin/server -c $id-config.xml test; then
			cat test.log
			doexit
		fi
	fi
}

function server_shutdown {
	echo "shutdown"|$base/bin/client || doexit
}

function client_set {
	echo "set $1=$2" | $base/bin/client || doexit
}

function client_get {
	echo "get $1" | ( $base/bin/client || doexit ) | sed -e 's/.*= //'
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
	diff -u $id.log.correct $id.log.test
}
