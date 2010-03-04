package DummyModule;
use Hector;

sub new {
	print STDERR "new()\n";
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = {};
	bless($self, $class);
	return $self;
}

sub DESTROY {
	print STDERR "DESTROY()\n";
}

sub Init {
	my ($self, $params) = @_;
	my @a;
	foreach my $p (@{$params}) {
		push(@a, $p->[0].'='.$p->[1]);
	}
	print STDERR "Init(".join(", ", @a).")\n";
	return 1;
}

sub getType {
	my ($self) = @_;
	print STDERR "getType()\n";
	return 0; #FIXME
}

sub getValueSync {
	my ($self, $name) = @_;
	print STDERR "getValueSync($name)\n";
	return "testvalue";
}

sub setValueSync {
	my ($self, $name, $value) = @_;
	print STDERR "setValueSync($name, $value)\n";
	return 1;
}

sub listNamesSync {
	my ($self) = @_;
	print STDERR "listNamesSync()\n";
	return [ "name1", "name2" ];
}

sub SaveCheckpoint {
	my ($self, $path, $id) = @_;
	print STDERR "SaveCheckpoint($path, $id)\n";
}

sub RestoreCheckpoint {
	my ($self, $path, $id) = @_;
	print STDERR "RestoreCheckpoint($path, $id)\n";
}

sub Process() {
	my ($self, $resource) = @_;

	printf STDERR "Process(%x)\n", $resource;

	return $resource+1;
}

1;
