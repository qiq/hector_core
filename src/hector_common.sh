# Useful bash functions to be used with hector_server and $HECTOR_CLIENT.
# This script should be included in other scripts.

if which perl >/dev/null; then
	HECTOR_SLEEP_COMMAND="perl -e 'select(undef, undef, undef, 0.25)';"
else
	HECTOR_SLEEP_COMMAND="sleep 1";
fi

if [ -z "$HECTOR_HOST" ]; then
	HECTOR_HOST="localhost:1100"
fi

function hector_server_start {
	if [ -z "$USE_VALGRIND" ]; then
		hector_server -c $@
	else
		libtool --mode=execute valgrind --tool=memcheck --track-origins=yes --leak-check=full --leak-resolution=high --num-callers=20 --trace-children=yes --log-file=hector_server.log.valgrind `which hector_server` -f -c $@ &
	fi
}

function hector_server_shutdown {
	echo "shutdown" | ( hector_client $HECTOR_HOST || exit )
}

function hector_client_set {
	echo "set $1=$2" | ( hector_client $HECTOR_HOST || exit )
}

function hector_client_get {
	echo "get $1" | ( hector_client $HECTOR_HOST || exit ) | sed -e 's/.*= //'
}

function hector_client_get_dontfail {
	echo "get $1" | hector_client $HECTOR_HOST 2>/dev/null | sed -e 's/.*= //'
}

function hector_client_wait {
	x=`hector_client_get $1`;
	while [ -z "$x" -o "$x" != "$2" ]; do
		echo -n "$x";
		bash -c "$HECTOR_SLEEP_COMMAND"
		x=`hector_client_get $1`;
		echo -ne "\033[100D\033[K";
	done
}

function hector_client_wait_lower {
	x=`hector_client_get $1`;
	while [ -z "$x" -o "0$x" -lt "$2" ]; do
		echo -n "$x";
		bash -c "$HECTOR_SLEEP_COMMAND"
		x=`hector_client_get $1`;
		echo -ne "\033[100D\033[K";
	done
}

function hector_client_wait_dontfail {
	x=`hector_client_get_dontfail $1`
	while [ -z "$x" -o "$x" != "$2" ]; do
		echo -n "$x";
		bash -c "$HECTOR_SLEEP_COMMAND"
		x=`hector_client_get_dontfail $1`
		echo -ne "\033[100D\033[K";
	done
}

function hector_client_save_checkpoint {
	echo "save_checkpoint $1" | ( hector_client $HECTOR_HOST || exit )
}

function hector_client_restore_checkpoint {
	echo "restore_checkpoint $1" | ( hector_client $HECTOR_HOST || exit )
}
