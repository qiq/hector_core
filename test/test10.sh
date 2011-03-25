#!/usr/bin/env bash

. test_common.sh

id=test10
rm -f $id.data.out
ln -s "$test_base/test/$id.data.in" . 2>/dev/null
test_server_start $id
hector_client_set PE_test.run 1
hector_client_wait M_save[0].items 1000
test_server_shutdown

md5sum <$id.data.out >$id.log.result
if [ -L $id.data.in ]; then
	rm $id.data.in
fi
test_compare_result $id
exit $?
