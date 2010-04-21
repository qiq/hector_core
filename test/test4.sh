#!/bin/bash

. common.sh

doinit
server_start
client_set robot_processing_engine.run 1
total=0
while [ $total -lt 5000 ]; do
	i0=`client_get M_save_resource[0].items`
	i1=`client_get M_save_resource[1].items`
	i2=`client_get M_save_resource[2].items`
	i3=`client_get M_save_resource[3].items`
	i4=`client_get M_save_resource[4].items`
	total=`echo $i0+$i1+$i2+$i3+$i4|bc`
done
client_set robot_processing_engine.run 0
server_shutdown

# M_simple[0]: Processing TestResource (1-69)
grep "Processing TestResource " test.log|sed -e 's|M_simple\[[0-9]\+\]: Processing TestResource (\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.test

compare_result
exit $?
