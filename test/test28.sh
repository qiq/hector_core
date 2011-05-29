#!/usr/bin/env bash

. test_common.sh

id=test28
test_server_batch $id test
# M_output[0]: Resource arrived (i1-0-253)
grep "Resource arrived " $id.log|sed -e 's|M_output\[[0-9]\+\]:.*Resource arrived (\([-a-zA-Z0-9]*\))|\1|'|sort -u|sort -n >$id.log.result
test_compare_result $id
exit $?
