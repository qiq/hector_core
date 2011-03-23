# TestPerl.pm, simple, perl
# Test Get/Set methods of perlTestResource.pm
# 
# Dependencies: none
# Parameters:
# items		r/o	Total items processed
# foo		r/w	Test string
# flipStatus	r/w	Whether to change status from 1 to 0 (or vice-versa)
# setStatus	r/w	Whether to set status to an arbitrary value (setStatus >= 0)

package TestPerl;

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

sub GetType {
	my ($self) = @_;
	return $Hector::Module::SIMPLE;
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

sub ProcessSimple() {
	my ($self, $resource) = @_;

	if (not defined $resource or $resource->GetTypeString(0) ne 'TestPerlResource') {
		$self->{'_object'}->log_error("Invalid resource: ".$resource->GetTypeString(0));
		return $resource;
	}
	$resource->SetAttrString("a", "xyz");
	$self->{'_object'}->log_info("xyz = ".$resource->GetAttrString("a"));
	$self->{'items'}++;

	return $resource;
}

1;
