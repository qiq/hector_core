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

# M_simple[0]: Processing TestResource 0-28
grep "Processing TestResource " $id.log|sed -e 's|M_simple\[[0-9]\+\]: Processing TestResource (\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.test

compare
