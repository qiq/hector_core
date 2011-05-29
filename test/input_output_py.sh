#!/usr/bin/env bash

. test_common.sh

id=input_output_py
test_server_batch $id test
grep "Resource arrived " $id.log|sed -e 's|M_output\[[0-9]\+\]:.*Resource arrived (\([-0-9]*\))|\1|'|sort -n >$id.log.result
test_compare_result $id
exit $?
