#!/usr/bin/env bash

. test_common.sh

id=test30
rm -f $id.data.out
test_server_batch $id test "$test_base/test/$id.data.in" $id.data.out

md5sum <$id.data.out >$id.log.result
test_compare_result $id
exit $?
