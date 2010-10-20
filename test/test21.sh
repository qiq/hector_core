#!/bin/bash

. test_common.sh

test_init
test_server_start
hector_client_set PE_test.run 1
echo -ne "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n"|hector_client -c "PROCESS PE_test" >/dev/null
hector_client_wait M_simple[0].items 10 
hector_client_set PE_test.run 0
hector_server_shutdown

grep "Processing TestResource " test.log|sed -e 's|M_simple\[[0-9]\+\]: Processing TestResource (\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.test
test_finish
test_compare_result
exit $?
