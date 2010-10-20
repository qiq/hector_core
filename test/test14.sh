#!/bin/bash

. test_common.sh

test_init
test_server_start
hector_client_set PE_test.run 1
total=0
while [ $total -lt 5000 ]; do
	i0=`hector_client_get M_output[0].items`
	i1=`hector_client_get M_output[1].items`
	i2=`hector_client_get M_output[2].items`
	i3=`hector_client_get M_output[3].items`
	i4=`hector_client_get M_output[4].items`
	total=`echo $i0+$i1+$i2+$i3+$i4|bc`
done
hector_client_set PE_test.run 0
hector_server_shutdown

# M_simple[0]: Processing TestResource (1-69)
grep "Processing TestResource " test.log|sed -e 's|M_simple\[[0-9]\+\]: Processing TestResource (\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.test
test_finish
test_compare_result
exit $?
