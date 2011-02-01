# TestOutput.pm, output, perl
# Just print out a message that resource arrived.
# 
# Dependencies: none
# Parameters:
# items		r/o	Total items processed

package TestOutput;

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
		}
	}
	return 1;
}

sub getType {
	my ($self) = @_;
	return $Hector::Module::OUTPUT;
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
	} else {
		$self->{'_object'}->log_error("Invalid value name: $name");
		return undef;
	}
}

sub SetValue {
	my ($self, $name, $value) = @_;
	if (exists $self->{$name}) {
		$self->{$name} = $value;
	} else {
		$self->{'_object'}->log_error("Invalid value name: $name");
		return 0;
	}
	return 1;
}

sub ListNames {
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

sub ProcessOutput() {
	my ($self, $resource) = @_;

	if (not defined $resource or $resource->getTypeStr() ne 'TestResource') {
		$self->{'_object'}->log_error("Invalid resource: ".$resource->getTypeStr());
	} else {
		$self->{'_object'}->log_info($resource->toStringShort()." Resource arrived (".$resource->getStr().")");
		$self->{'items'}++;
	}
	return $resource;
}

1;
