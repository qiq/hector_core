package TestSimple;

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
		'foo' => 0,
		'filpStatus' => 0,
		'setStatus' => -1,
	};
	bless($self, $class);
	return $self;
}

sub DESTROY {
}

sub Init {
	my ($self, $params) = @_;

	# second stage?
	return 1 if (not defined $params);

	foreach my $p (@{$params}) {
		if (exists $self->{$p->[0]}) {
			$self->{$p->[0]} = $p->[1];
		} elsif ($p->[0] eq 'alias') {
			$self->{'foo'} = $p->[1];
		}
	}
	return 1;
}

sub getType {
	my ($self) = @_;
	return $Hector::Module::SIMPLE;
}

sub getValueSync {
	my ($self, $name) = @_;
	if (exists $self->{$name}) {
		return $self->{$name};
	} elsif ($name eq 'alias') {
		return $self->{'foo'};
	} else {
		$self->{'_object'}->log_error("Invalid value name: $name");
		return undef;
	}
}

sub setValueSync {
	my ($self, $name, $value) = @_;
	if (exists $self->{$name}) {
		$self->{$name} = $value;
	} elsif ($name eq 'alias') {
		$self->{'foo'} = $value;
	} else {
		$self->{'_object'}->log_error("Invalid value name: $name");
		return 0;
	}
	return 1;
}

sub listNamesSync {
	my ($self) = @_;
	return [ grep { $_ !~ /^_/ } keys %{$self}, 'alias' ];
}

sub SaveCheckpoint {
	my ($self, $path, $id) = @_;
	$self->{'_object'}->log_info("SaveCheckpoint($path, $id)");
}

sub RestoreCheckpoint {
	my ($self, $path, $id) = @_;
	$self->{'_object'}->log_info("RestoreCheckpoint($path, $id)");
}

sub ProcessSimple() {
	my ($self, $resource) = @_;

	if (not defined $resource or $resource->getTypeStr() ne 'TestResource') {
		$self->{'_object'}->log_error("Invalid resource: ".$resource->getTypeStr());
		return $resource;
	}
	$self->{'_object'}->log_info("Processing TestResource (".$resource->getStr().")");
	$resource->setStatus($resource->getStatus() == 0 ? 1 : 0) if ($self->{'flipStatus'});
	$resource->setStatus($self->{'setStatus'}) if ($self->{'setStatus'} >= 0);
	$self->{'items'}++;

	return $resource;
}

1;
