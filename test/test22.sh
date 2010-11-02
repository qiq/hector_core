#!/bin/bash

. test_common.sh

test_init
test_server_start
hector_client_set PE_test.run 1
hector_client_set PE_second.run 1
hector_client_wait M_output[0].items 1000
hector_client_set PE_test.run 0
hector_client_set PE_second.run 0
hector_server_shutdown

grep "\(Resource arrived \|M_dump2\[0\]: TestResource\)" test.log|sed -e 's|M_output\[[0-9]\+\]: Resource arrived (\([-0-9]*\))|\1|'|sed -e 's|M_dump2\[0\]: TestResource.*: \(.*\)|\1|'|sort -n >$id.log.test
test_finish
test_compare_result
exit $?
