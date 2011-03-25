#!/bin/bash

. test_common.sh

test_init
test_server_batch

grep "Resource arrived" test.log|sed -e 's|M_output\[[0-9]\+\]: \[TPerlR \([0-9]\+\) 0\] Resource arrived|\1|'|sort -n >$id.log.test
test_finish
test_compare_result
exit $?
