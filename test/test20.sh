#!/bin/bash

. common.sh

test_init
rm -f test20.data.out
ln -s $base/test/test20.data.in . 2>/dev/null
test_server_start
hector_client_set test_processing_engine.run 1
hector_client_wait M_save[0].items 2000
hector_server_shutdown

md5sum <test20.data.out >$id.log.test
if [ -L test20.data.in ]; then
	rm test20.data.in
fi
test_finish
test_compare_result
exit $?
