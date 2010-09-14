#!/bin/bash

. common.sh

rm -f test10.data.out

test_init
test_server_start
hector_client_set test_processing_engine.run 1
hector_client_wait M_save[0].items 1000
hector_server_shutdown

md5sum <test10.data.out >$id.log.test
test_finish
test_compare_result
exit $?
