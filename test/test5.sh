#!/bin/bash

# simple test: one reader, one writer, more threads threaded

. common.sh
prepare
run

$base/bin/client <<-EOT;
set robot_processing_engine.run = 1
EOT
sleep 2
$base/bin/client <<-EOT;
set robot_processing_engine.run = 0
shutdown
EOT

# M_save_resource[0]: Resource arrived (i1-0-253)
grep "Resource arrived " $id.log|sed -e 's|M_save_resource\[[0-9]\+\]: Resource arrived (\([-a-zA-Z0-9]*\))|\1|'|sort -u|sort -n >$id.log.test

compare
