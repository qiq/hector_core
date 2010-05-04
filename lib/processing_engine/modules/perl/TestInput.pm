package TestInput;

use warnings;
use strict;
use Hector;

sub new {
	my ($proto, $id, $threadIndex) = @_;
	my $class = ref($proto) || $proto;
	my $self = {
		'_id' => $id,
		'_threadIndex' => $threadIndex,
		'items' => 0,
		'maxItems' => 0,
		'idPrefix' => undef,
	};
	bless($self, $class);
	return $self;
}

sub DESTROY {
}

sub Init {
	my ($self, $params) = @_;
	my @a;
	foreach my $p (@{$params}) {
		if (exists $self->{$p->[0]}) {
			$self->{$p->[0]} = $p->[1];
		}
	}
	return 1;
}

sub getType {
	my ($self) = @_;
	return $Hector::Module::INPUT;
}

sub getValueSync {
	my ($self, $name) = @_;
	if (exists $self->{$name}) {
		return $self->{$name};
	} else {
		print STDERR "Invalid value name: $name\n";
		return undef;
	}
}

sub setValueSync {
	my ($self, $name, $value) = @_;
	if (exists $self->{$name}) {
		$self->{$name} = $value;
	} else {
		print STDERR "Invalid value name: $name\n";
		return 0;
	}
	return 1;
}

sub listNamesSync {
	my ($self) = @_;
	return [ grep { $_ !~ /^_/ } keys %{$self} ];
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

	if (defined $resource) {
		print STDERR "TestInput: resource is already defined.\n";
		return undef;
	}
	return undef if ($self->{'maxItems'} and $self->{'items'} >= $self->{'maxItems'});
	$resource = Hector::TestResource->new();
	$resource->setId($self->{'_threadIndex'}*10000+$self->{'items'});
	$resource->setStr(sprintf("%s%d-%d", defined $self->{'idPrefix'} ? $self->{'idPrefix'} : "", $self->{'_threadIndex'}, $self->{'items'}++));
	print STDERR "Loading resource (".$resource->getStr().")\n";

	return $resource;
}

sub ProcessMulti() {
	my ($self, $inputResources, $outputResources) = @_;

	print STDERR "ProcessMulti() is not implemented\n";

	return 1;
}

1;
