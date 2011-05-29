#!/usr/bin/env bash

. test_common.sh

id=input_multi_output_pl
test_server_batch $id test
grep "Resource arrived " $id.log|sed -e 's|M_output\[[0-9]\+\]:.*Resource arrived (\([-a-zA-Z0-9]*\))|\1|' >$id.log.result
test_compare_result $id
exit $?
