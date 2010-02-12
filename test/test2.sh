#!/bin/bash

# simple test: one reader, one writer, more threads threaded

base=`dirname "$0"`
cd $base
base=`readlink -f "$base"`'/../src'
base=`readlink -f "$base"`
id=`basename "$0" .sh`

export LD_LIBRARY_PATH=$base/lib/.libs:$LD_LIBRARY_PATH

rm -f $id.log
$base/bin/server -c $id-config.xml $id
sleep 1
$base/bin/client <<-EOT;
set robot_processing_engine.run = 1
EOT
sleep 3
$base/bin/client <<-EOT;
set robot_processing_engine.run = 0
shutdown
EOT

grep "^Resource arrived " $id.log|sed -e 's|Resource arrived (URL: http://test.com/?id=\([-0-9]*\))|\1|'|sort -u|sort -n >$id.log.test
diff -u $id.log.correct $id.log.test
