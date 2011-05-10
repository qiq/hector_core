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
	my $self = $class->SUPER::new($object, $id, $threadIndex);

	$self->{'items'} = 0;

	bless($self, $class);
	return $self;
}

sub DESTROY {
}

sub GetType {
	my ($self) = @_;
	return $Hector::Module::SIMPLE;
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
