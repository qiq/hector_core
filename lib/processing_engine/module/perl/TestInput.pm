# TestInput.pm, input, perl
# Create TestResource resources.
# 
# Dependencies: none
# Parameters:
# items		r/o	Total items processed
# maxItems	init	Number of resources to generate
# idPrefix	r/w	Prefix to be used as prefix for string in TestResource

package TestInput;

use warnings;
use strict;
use Hector;
use Module;
our @ISA = qw(Module);

sub new {
	my ($proto, $object, $id, $threadIndex) = @_;
	my $class = ref($proto) || $proto;
	my $self = $class->SUPER::new('INPUT', $object, $id, $threadIndex);

	$self->{'items'} = 0;
	$self->{'maxItems'} = 0;
	$self->{'idPrefix'} = undef;

	$self->{'_typeId'} = &Hector::Resource::GetRegistry()->NameToId("TestResource");

	bless($self, $class);
	return $self;
}

sub DESTROY {
}

sub ProcessInput() {
	my ($self, $resource) = @_;

	if (defined $resource) {
		$self->LOG_ERROR($resource, "TestInput: resource is already defined.");
		return undef;
	}
	return undef if ($self->{'maxItems'} and $self->{'items'} >= $self->{'maxItems'});
	$resource = &Hector::ResourceToTestResource(&Hector::Resource::GetRegistry()->AcquireResource($self->{'_typeId'}));
	$resource->SetId($self->{'_threadIndex'}*10000+$self->{'items'});
	$resource->SetStr(sprintf("%s%d-%d", defined $self->{'idPrefix'} ? $self->{'idPrefix'} : "", $self->{'_threadIndex'}, $self->{'items'}));
	$self->LOG_INFO($resource, "Loading resource (".$resource->GetStr().")");
	$self->{'items'}++;

	return $resource;
}

1;
