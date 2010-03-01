#!/bin/bash

# simple test: one reader, one writer, single threaded

. common.sh

init
run_server
client_set robot_processing_engine.run 1
client_wait M_save_resource[0].items 1000
client_set robot_processing_engine.run 0
shutdown_server

grep "Resource arrived " $id.log|sed -e 's|M_save_resource\[[0-9]\+\]: Resource arrived (\([-0-9]*\))|\1|'|sort -n >$id.log.test

compare_result $id.log.test $id.log.correct
exit $?
