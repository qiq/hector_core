#!/usr/bin/env bash

. test_common.sh

id=test34
test_server_batch $id test
grep "Resource arrived" $id.log|sed -e 's|M_output\[[0-9]\+\]: \[TPerlR \([0-9]\+\) 0\] Resource arrived|\1|'|sort -n >$id.log.result
test_compare_result $id
exit $?
