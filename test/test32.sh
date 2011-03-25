#!/usr/bin/env bash

. test_common.sh

id=test32
test_server_batch $id
grep "\(Resource arrived \|M_dump2\[0\]:\)" $id.log|sed -e 's|M_output\[[0-9]\+\]:.*Resource arrived (\([-0-9]*\))|\1|'|sed -e 's|M_dump2\[0\]:.* \(.*\)|\1|'|sort -n >$id.log.result
test_compare_result $id
exit $?
