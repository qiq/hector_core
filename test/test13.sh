#!/usr/bin/env bash

. test_common.sh

id=test13
test_server_batch $id
# M_simple[0]: Processing TestResource 0-28
grep "Processing " $id.log|sed -e 's|M_simple\[[0-9]\+\]:.*Processing (\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.result
test_compare_result $id
exit $?
