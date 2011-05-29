#!/usr/bin/env bash

. test_common.sh

id=inputs_simple_output_mt_py
test_server_batch $id test
grep "Resource arrived " $id.log|sed -e 's|M_output\[[0-9]\+\]:.*Resource arrived (\([-a-zA-Z0-9]*\))|\1|'|sort -u|sort -n >$id.log.result
test_compare_result $id
exit $?
