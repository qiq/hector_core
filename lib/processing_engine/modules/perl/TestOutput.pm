package TestOutput;

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
	return $Hector::Module::OUTPUT;
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

	print STDERR "Resource arrived (".$resource->getStr().")\n";
	$self->{'items'}++;
	return undef;
}

sub ProcessMulti() {
	my ($self, $inputResources, $outputResources) = @_;

	print STDERR "ProcessMulti() is not implemented\n";

	return 1;
}

1;
