package TestMulti;

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
		'_resources' => [],
		'items' => 0,
		'foo' => 0,
	};
	bless($self, $class);
	return $self;
}

sub DESTROY {
}

sub Init {
	my ($self, $params) = @_;
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
	return $Hector::Module::MULTI;
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

my $MAX_RESOURCES = 100;
sub ProcessMulti() {
	my ($self, $inputResources, $outputResources) = @_;

	return 0 if (not defined $inputResources or not defined $outputResources);

	while (@{$inputResources} > 0 and @{$self->{'_resources'}} <= $MAX_RESOURCES) {
		my $resource = shift(@{$inputResources});
		push(@{$self->{'_resources'}}, $resource) if ($resource->getTypeStr() eq 'TestResource');
	}

	return -1 if (@{$self->{'_resources'}} == 0);

	select(undef, undef, undef, 0.0001);

	my $resource = shift(@{$self->{'_resources'}});
	push(@{$outputResources}, $resource);
	$self->{'_object'}->log_info("Processed TestResource (".$resource->getStr().")");
	$self->{'items'}++;

	return $MAX_RESOURCES-@{$self->{'_resources'}};
}

1;
