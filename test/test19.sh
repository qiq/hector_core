#!/bin/bash

. common.sh

doinit
server_start
client_set robot_processing_engine.run 1
client_wait M_save_resource[0].items 1000
server_shutdown

# M_save_resource[0]: Resource arrived (i1-0-253)
grep "Resource arrived " test.log|sed -e 's|M_save_resource\[[0-9]\+\]: Resource arrived (\([-a-zA-Z0-9]*\))|\1|' >$id.log.test

compare_result
exit $?
