#!/bin/bash

. test_common.sh

test_init
rm -f test21.data.out
ln -s $base/test/test21.data.in . 2>/dev/null
test_server_start
hector_client_set PE_test.run 1
hector_client -c "PROCESS PE_test" <test21.data.in >test21.data.out
hector_client_wait M_simple[0].items 1000
hector_client_set PE_test.run 0
hector_server_shutdown

md5sum <test21.data.out >$id.log.test
if [ -L test21.data.in ]; then
	rm test21.data.in
fi
test_finish
test_compare_result
exit $?
