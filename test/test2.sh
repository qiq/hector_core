#!/bin/bash

# simple test: one reader, one writer, more threads threaded

. common.sh
prepare
run

$base/bin/client <<-EOT;
set robot_processing_engine.run = 1
EOT
sleep 3
$base/bin/client <<-EOT;
set robot_processing_engine.run = 0
shutdown
EOT

grep "Resource arrived " $id.log|sed -e 's|save_resource_module: Resource arrived (URL: http://test.com/?id=\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.test

compare
