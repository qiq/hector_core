#!/bin/bash

# simple test: one reader, one writer, single threaded

if which perl >/dev/null; then
	SLEEP="perl -e 'select(undef, undef, undef, 0.25)';"
else
	SLEEP="sleep 1";
fi

function init {
	if [ $0 == '-bash' ]; then
		echo "Include please in a test script please"
		return
	fi
	base=`dirname "$0"`
	cd $base
	base=`readlink -f "$base"`'/../src'
	base=`readlink -f "$base"`
	id=`basename "$0" .sh`

	export LD_LIBRARY_PATH=$base/lib/.libs:$LD_LIBRARY_PATH

	rm -f test.log
}

function run_server {
	if ! $base/bin/server -c $id-config.xml test; then
		cat $id.log
		exit 1;
	fi
}

function shutdown_server {
	echo "shutdown"|$base/bin/client || exit 1
}

function client_set {
	echo "set $1=$2" | $base/bin/client || exit 1
}

function client_get {
	echo "get $1" | ( $base/bin/client || exit 1 ) | sed -e 's/.*= //'
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
