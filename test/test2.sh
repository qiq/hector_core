#!/usr/bin/env bash

. test_common.sh

id=test2
test_server_batch $id
grep "Resource arrived " $id.log|sed -e 's|M_output\[[0-9]\+\]:.*Resource arrived (\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.result
test_compare_result $id
exit $?
