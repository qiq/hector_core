#!/usr/bin/env bash

. test_common.sh

id=process
rm -f $id.out
ln -s "$test_base/test/$id.data.in" . 2>/dev/null
test_server_start $id test
pid=$!
hector_client_set PE_test.run 1
hector_client -c "PROCESS PE_test" $HECTOR_HOST <$id.data.in >$id.data.out
hector_client_wait M_simple[0].items 1000
hector_client_set PE_test.run 0
test_server_shutdown
wait $!

md5sum <$id.data.out >$id.log.result
if [ -L $id.data.in ]; then
	rm $id.data.in
fi
test_compare_result $id
exit $?
