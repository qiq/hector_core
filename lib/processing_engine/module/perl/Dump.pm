# Dump.pm, simple, perl
# Dumps Resource content into log. Useful for debugging purposes.
# 
# Dependencies:
# Parameters:

package Dump;

use warnings;
use strict;
use Hector;
use Module;
our @ISA = qw(Module);

sub new {
	my ($proto, $object, $id, $threadIndex) = @_;
	my $class = ref($proto) || $proto;
	my $self = $class->SUPER::new('SIMPLE', $object, $id, $threadIndex);

	bless($self, $class);
	return $self;
}

sub DESTROY {
}

sub ProcessSimple() {
	my ($self, $resource) = @_;

	$self->LOG_DEBUG($resource->ToString($Hector::Object::INFO));

	return $resource;
}

1;
