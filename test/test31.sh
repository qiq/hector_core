#!/bin/bash

. test_common.sh

test_init
rm -f test31.data.out
ln -s $base/test/test31.data.in . 2>/dev/null
test_server_start
hector_client_set PE_test.run 1
hector_client -c "PROCESS PE_test" $HECTOR_HOST <test31.data.in >test31.data.out
hector_client_wait M_simple[0].items 1000
hector_client_set PE_test.run 0
hector_server_shutdown

md5sum <test31.data.out >$id.log.test
if [ -L test31.data.in ]; then
	rm test31.data.in
fi
test_finish
test_compare_result
exit $?
