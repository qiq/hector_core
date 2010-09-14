# Useful bash functions to be used with hector_server and hector_client.
# This script should be include in other scripts.

if which perl >/dev/null; then
	HECTOR_SLEEP_COMMAND="perl -e 'select(undef, undef, undef, 0.25)';"
else
	HECTOR_SLEEP_COMMAND="sleep 1";
fi

function hector_server_start {
	if [ $# -lt 2 ]; then
		echo "usage: server_start config_file server_id"
		return;
	fi
	config_file=$1
	server_id=$2;
	if [ "$USE_VALGRIND" == 1 ]; then
		libtool --mode=execute valgrind --tool=memcheck --track-origins=yes --leak-check=full --leak-resolution=high --num-callers=20 --trace-children=yes --log-file=hector_server.log.valgrind hector_server -c ${config_file} -f ${server_id} &
		hector_client_wait_dontfail robot_processing_engine.run 0
	else
		hector_server -c ${config_file} ${server_id}
	fi
}

function hector_server_shutdown {
	echo "shutdown" | ( hector_client || exit )
}

function hector_client_set {
	echo "set $1=$2" | ( hector_client || exit )
}

function hector_client_get {
	echo "get $1" | ( hector_client || exit ) | sed -e 's/.*= //'
}

function hector_client_get_dontfail {
	echo "get $1" | hector_client 2>/dev/null | sed -e 's/.*= //'
}

function hector_client_wait {
	x=`hector_client_get $1`;
	while [ -z "$x" -o "$x" != "$2" ]; do
		bash -c "$HECTOR_SLEEP_COMMAND"
		x=`hector_client_get $1`;
		echo -ne "."
	done
}

function hector_client_wait_dontfail {
	x=`hector_client_get_dontfail $1`
	while [ -z "$x" -o "$x" != "$2" ]; do
		bash -c "$HECTOR_SLEEP_COMMAND"
		x=`hector_client_get_dontfail $1`
		echo -ne "."
	done
}
