#!/bin/bash

. test_common.sh

test_init
rm -f test30.data.out
ln -s $base/test/test30.data.in . 2>/dev/null
test_server_start test30.data.in test30.data.out
hector_client_set PE_test.run 1
hector_client_wait M_save[0].items 2000
hector_server_shutdown

md5sum <test30.data.out >$id.log.test
if [ -L test30.data.in ]; then
	rm test30.data.in
fi
test_finish
test_compare_result
exit $?
