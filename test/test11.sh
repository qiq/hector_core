#!/bin/bash

. common.sh

test_init
test_server_start
hector_client_set robot_processing_engine.run 1
hector_client_wait M_save_resource[0].items 1000
hector_client_set robot_processing_engine.run 0
hector_server_shutdown

grep "Resource arrived " test.log|sed -e 's|M_save_resource\[[0-9]\+\]: Resource arrived (\([-0-9]*\))|\1|'|sort -n >$id.log.test
test_finish
test_compare_result
exit $?
