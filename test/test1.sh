#!/bin/bash

# simple test: one reader, one writer, single threaded

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

grep "Resource arrived " $id.log|sed -e 's|M_save_resource\[[0-9]\+\]: Resource arrived (\([-0-9]*\))|\1|'|sort -n >$id.log.test

compare
