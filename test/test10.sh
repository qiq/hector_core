#!/bin/bash

. test_common.sh

test_init
rm -f test10.data.out
ln -s $base/test/test10.data.in . 2>/dev/null
test_server_start
hector_client_set test_processing_engine.run 1
hector_client_wait M_save[0].items 1000
hector_server_shutdown

md5sum <test10.data.out >$id.log.test
if [ -L test10.data.in ]; then
	rm test10.data.in
fi
test_finish
test_compare_result
exit $?
