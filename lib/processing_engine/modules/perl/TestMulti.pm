# TestMulti.pm, multi, perl
# Process resources using ProcessMulti()
# 
# Dependencies: none
# Parameters:
# items		r/o	Total items processed
# foo		r/w	Test string
# timeTick	r/w	Max time to spend in ProcessMulti()

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
		'timeTick' => 1000,
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
	return $Hector::Module::MULTI;
}

sub Start {
	my ($self) = @_;
}

sub Stop {
	my ($self) = @_;
}

sub Pause {
	my ($self) = @_;
}

sub Resume {
	my ($self) = @_;
}

sub GetValue {
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

sub SetValue {
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

sub ListNames {
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

	return (0, $MAX_RESOURCES) if (not defined $inputResources or not defined $outputResources);

	while (@{$inputResources} > 0 and @{$self->{'_resources'}} <= $MAX_RESOURCES) {
		my $resource = shift(@{$inputResources});
		if ($resource->getTypeStr() eq 'TestResource') {
			push(@{$self->{'_resources'}}, $resource);
		} else {
			push(@{$outputResources}, $resource);
		}
	}

	return (0, $MAX_RESOURCES) if (@{$self->{'_resources'}} == 0);

	select(undef, undef, undef, $self->{'timeTick'}/1000000);

	my $resource = shift(@{$self->{'_resources'}});
	push(@{$outputResources}, $resource);
	$self->{'_object'}->log_info($resource->toStringShort()." Processed (".$resource->getStr().")");
	$self->{'items'}++;

	return (scalar @{$self->{'_resources'}}, $MAX_RESOURCES-@{$self->{'_resources'}});
}

1;
