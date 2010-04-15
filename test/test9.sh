#!/bin/bash

. common.sh

init
server_start
client_set robot_processing_engine.run 1
client_wait M_save_resource[0].items 1000
server_shutdown

# M_save_resource[0]: Resource arrived (i1-0-253)
grep "Resource arrived " test.log|sed -e 's|M_save_resource\[[0-9]\+\]: Resource arrived (\([-a-zA-Z0-9]*\))|\1|'|sort -u|sort -n >$id.log.test

compare_result $id.log.test $id.log.correct
exit $?
