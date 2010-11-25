#!/bin/bash

. test_common.sh

test_init
test_server_start
hector_client_set PE_test.run 1
hector_client_wait M_output[0].items 100
hector_server_shutdown

grep "Resource arrived " test.log|sed -e 's|M_output\[[0-9]\+\]:.*Resource arrived (\([-a-zA-Z0-9]*\))|\1|' >$id.log.test
test_finish
test_compare_result
exit $?
