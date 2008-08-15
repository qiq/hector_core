EXTRA_DIST = ModuleDummy.pl

all-local: ProtoBuf/Resource.pm

ProtoBuf/Resource.pm: ../Resource.proto
	/home/qiq/protobuf-perl/bin/protoc -I.. ../Resource.proto --perl_out=.

clean-local:
	rm -f ProtoBuf/Resource.pm
