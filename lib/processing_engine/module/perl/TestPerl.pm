# TestPerl.pm, simple, perl
# Test Get/Set methods of perlTestResource.pm
# 
# Dependencies: none
# Parameters:
# items		r/o	Total items processed

package TestPerl;

use warnings;
use strict;
use Hector;
use Module;
our @ISA = qw(Module);

sub new {
	my ($proto, $object, $id, $threadIndex) = @_;
	my $class = ref($proto) || $proto;
	my $self = $class->SUPER::new('SIMPLE', $object, $id, $threadIndex);

	$self->{'items'} = 0;

	bless($self, $class);
	return $self;
}

sub ProcessSimple() {
	my ($self, $resource) = @_;

	if (not defined $resource or $resource->GetTypeString(0) ne 'TestPerlResource') {
		$self->LOG_ERROR("Invalid resource: ".$resource->GetTypeString(0));
		return $resource;
	}
	$resource->SetAttrString("a", "xyz");
	$self->LOG_INFO($resource, "xyz = ".$resource->GetAttrString("a"));
	$self->{'items'}++;

	return $resource;
}

1;
