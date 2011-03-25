#!/bin/bash

. test_common.sh

test_init
test_server_batch

grep "Processing " test.log|sed -e 's|M_simple\[[0-9]\+\]:.*Processing (\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.test
test_finish
test_compare_result
exit $?
