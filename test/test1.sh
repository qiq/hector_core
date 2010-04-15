#!/bin/bash

. common.sh

init
server_start
client_set robot_processing_engine.run 1
client_wait M_save_resource[0].items 1000
client_set robot_processing_engine.run 0
server_shutdown

grep "Resource arrived " test.log|sed -e 's|M_save_resource\[[0-9]\+\]: Resource arrived (\([-0-9]*\))|\1|'|sort -n >$id.log.test

compare_result $id.log.test $id.log.correct
exit $?
