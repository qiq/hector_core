#!/bin/sh
cd `dirname $0`
tee /tmp/in | ./ModuleDummy.pl
