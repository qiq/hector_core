#!/bin/bash

. test_common.sh

test_init
test_server_start
hector_client_set PE_test.run 1
hector_client_wait M_output[0].items 1000
hector_client_set PE_test.run 0
hector_server_shutdown

# M_simple[0]: Processing TestResource 0-28
grep "Processing " test.log|sed -e 's|M_simple\[[0-9]\+\]:.*Processing (\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.test
test_finish
test_compare_result
exit $?
