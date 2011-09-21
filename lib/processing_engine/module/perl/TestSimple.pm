# TestSimple.pm, simple, perl
# Process resources using ProcessSimple()
# 
# Dependencies: none
# Parameters:
# items		r/o	Total items processed
# foo		r/w	Test string
# flipStatus	r/w	Whether to change status from 1 to 0 (or vice-versa)
# setStatus	r/w	Whether to set status to an arbitrary value (setStatus >= 0)

package TestSimple;

use warnings;
use strict;
use Hector;
use Module;
our @ISA = qw(Module);

sub new {
	my ($proto, $object, $id, $threadIndex) = @_;
	my $class = ref($proto) || $proto;
	my $self = $class->SUPER::new($object, $id, $threadIndex);

	$self->{'items'} = 0;
	$self->{'foo'} = 0;
	$self->{'filpStatus'} = 0;
	$self->{'setStatus'} = -1;

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

sub GetValue {
	my ($self, $name) = @_;
	if (exists $self->{$name}) {
		return $self->{$name};
	} elsif ($name eq 'alias') {
		return $self->{'foo'};
	} else {
		$self->LOG_ERROR("Invalid value name: $name");
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
		$self->LOG_ERROR("Invalid value name: $name");
		return 0;
	}
	return 1;
}

sub ListNames {
	my ($self) = @_;
	return [ grep { $_ !~ /^_/ } keys %{$self}, 'alias' ];
}

sub ProcessSimple() {
	my ($self, $resource) = @_;

	if (not defined $resource or $resource->GetTypeString() ne 'TestResource') {
		$self->LOG_ERROR("Invalid resource: ".$resource->GetTypeString());
		return $resource;
	}
	$self->LOG_INFO($resource, "Processing (".$resource->GetStr().")");
	$resource->SetStatus($resource->GetStatus() == 0 ? 1 : 0) if ($self->{'flipStatus'});
	$resource->SetStatus($self->{'setStatus'}) if ($self->{'setStatus'} >= 0);
	$self->{'items'}++;

	return $resource;
}

1;
