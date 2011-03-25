#!/usr/bin/env bash

. test_common.sh

id=test33
test_server_start $id
hector_client_set PE_test.run 1
hector_client_wait M_output[0].items 1000
hector_client_set PE_test.run 0
test_server_shutdown
grep "Resource arrived" $id.log|sed -e 's|M_output\[[0-9]\+\]: \[TPerlR \([0-9]\+\) 0\] Resource arrived|\1|'|sort -n >$id.log.result
test_compare_result $id
exit $?
