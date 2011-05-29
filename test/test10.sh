#!/usr/bin/env bash

. test_common.sh

id=test10
rm -f $id.data.out
test_server_start $id test "$test_base/test/$id.data.in" $id.data.out
hector_client_set PE_test.run 1
hector_client_wait M_save[0].items 1000
test_server_shutdown

md5sum <$id.data.out >$id.log.result
test_compare_result $id
exit $?
