#!/usr/bin/env bash

. test_common.sh

id=input_simple_output
test_server_batch $id test
grep "Processing " $id.log|sed -e 's|M_simple\[[0-9]\+\]:.*Processing (\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.result
test_compare_result $id
exit $?
