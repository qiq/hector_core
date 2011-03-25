#!/bin/bash

. test_common.sh

test_init
test_server_batch

grep "\(Resource arrived \|M_dump2\[0\]:\)" test.log|sed -e 's|M_output\[[0-9]\+\]:.*Resource arrived (\([-0-9]*\))|\1|'|sed -e 's|M_dump2\[0\]:.* \(.*\)|\1|'|sort -n >$id.log.test
test_finish
test_compare_result
exit $?
