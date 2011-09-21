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
use Module;
our @ISA = qw(Module);

sub new {
	my ($proto, $object, $id, $threadIndex) = @_;
	my $class = ref($proto) || $proto;
	my $self = $class->SUPER::new($object, $id, $threadIndex);

	$self->{'items'} = 0;

	bless($self, $class);
	return $self;
}

sub DESTROY {
}

sub GetType {
	my ($self) = @_;
	return $Hector::Module::OUTPUT;
}

sub ProcessOutput() {
	my ($self, $resource) = @_;

	if (not defined $resource or $resource->GetTypeString() ne 'TestResource') {
		$self->LOG_ERROR("Invalid resource: ".$resource->GetTypeString());
	} else {
		$self->LOG_INFO($resource, "Resource arrived (".$resource->GetStr().")");
		$self->{'items'}++;
	}
	return $resource;
}

1;
