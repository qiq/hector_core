#!/bin/bash

. common.sh

test_init
test_server_start
hector_client_set robot_processing_engine.run 1
total=0
while [ $total -lt 5000 ]; do
	i0=`hector_client_get M_save_resource[0].items`
	i1=`hector_client_get M_save_resource[1].items`
	i2=`hector_client_get M_save_resource[2].items`
	i3=`hector_client_get M_save_resource[3].items`
	i4=`hector_client_get M_save_resource[4].items`
	total=`echo $i0+$i1+$i2+$i3+$i4|bc`
done
hector_client_set robot_processing_engine.run 0
hector_server_shutdown

grep "Resource arrived " test.log|sed -e 's|M_save_resource\[[0-9]\+\]: Resource arrived (\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.test
test_finish
test_compare_result
exit $?
