#!/bin/bash

. common.sh

init
run_server
client_set robot_processing_engine.run 1
client_wait M_save_resource[0].items 1000
client_set robot_processing_engine.run 0
shutdown_server

# M_simple[0]: Processing TestResource 0-28
grep "Processing TestResource " test.log|sed -e 's|M_simple\[[0-9]\+\]: Processing TestResource (\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.test

compare_result $id.log.test $id.log.correct
exit $?
