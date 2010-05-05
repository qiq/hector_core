package TestInput;

use warnings;
use strict;
use Hector;

sub new {
	my ($proto, $object, $id, $threadIndex) = @_;
	my $class = ref($proto) || $proto;
	my $self = {
		'_object' => $object,
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
		$self->{'_object'}->log_error("Invalid value name: $name");
		return undef;
	}
}

sub setValueSync {
	my ($self, $name, $value) = @_;
	if (exists $self->{$name}) {
		$self->{$name} = $value;
	} else {
		$self->{'_object'}->log_error("Invalid value name: $name");
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
	$self->{'_object'}->log_info("SaveCheckpoint($path, $id)");
}

sub RestoreCheckpoint {
	my ($self, $path, $id) = @_;
	$self->{'_object'}->log_info("RestoreCheckpoint($path, $id)");
}

sub Process() {
	my ($self, $resource) = @_;

	if (defined $resource) {
		$self->{'_object'}->log_error("TestInput: resource is already defined.");
		return undef;
	}
	return undef if ($self->{'maxItems'} and $self->{'items'} >= $self->{'maxItems'});
	$resource = Hector::TestResource->new();
	$resource->setId($self->{'_threadIndex'}*10000+$self->{'items'});
	$resource->setStr(sprintf("%s%d-%d", defined $self->{'idPrefix'} ? $self->{'idPrefix'} : "", $self->{'_threadIndex'}, $self->{'items'}++));
	$self->{'_object'}->log_info("Loading resource (".$resource->getStr().")");

	return $resource;
}

sub ProcessMulti() {
	my ($self, $inputResources, $outputResources) = @_;

	$self->{'_object'}->log_error("ProcessMulti() is not implemented");

	return 1;
}

1;
