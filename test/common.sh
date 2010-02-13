#!/bin/bash

# simple test: one reader, one writer, single threaded

function prepare {
	base=`dirname "$0"`
	cd $base
	base=`readlink -f "$base"`'/../src'
	base=`readlink -f "$base"`
	id=`basename "$0" .sh`

	export LD_LIBRARY_PATH=$base/lib/.libs:$LD_LIBRARY_PATH

	rm -f $id.log
}

function run {
	$base/bin/server -c $id-config.xml $id
	sleep 1;
}

function compare {
	diff -u $id.log.correct $id.log.test
}
