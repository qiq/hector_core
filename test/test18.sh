#!/bin/bash

. test_common.sh

test_init
test_server_batch

# M_output[0]: Resource arrived (i1-0-253)
grep "Resource arrived " test.log|sed -e 's|M_output\[[0-9]\+\]:.*Resource arrived (\([-a-zA-Z0-9]*\))|\1|'|sort -u|sort -n >$id.log.test
test_finish
test_compare_result
exit $?
