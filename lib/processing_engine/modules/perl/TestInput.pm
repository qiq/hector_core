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
	my $self = $class->SUPER::new($object, $id, $threadIndex);

	$self->{'items'} = 0;
	$self->{'maxItems'} = 0;
	$self->{'idPrefix'} = undef;

	bless($self, $class);
	return $self;
}

sub DESTROY {
}

sub GetType {
	my ($self) = @_;
	return $Hector::Module::INPUT;
}

sub ProcessInput() {
	my ($self, $resource) = @_;

	if (defined $resource) {
		$self->{'_object'}->log_error("TestInput: resource is already defined.");
		return undef;
	}
	return undef if ($self->{'maxItems'} and $self->{'items'} >= $self->{'maxItems'});
	$resource = Hector::TestResource->new();
	$resource->SetId($self->{'_threadIndex'}*10000+$self->{'items'});
	$resource->SetStr(sprintf("%s%d-%d", defined $self->{'idPrefix'} ? $self->{'idPrefix'} : "", $self->{'_threadIndex'}, $self->{'items'}));
	$self->{'_object'}->log_info($resource->ToStringShort()." Loading resource (".$resource->GetStr().")");
	$self->{'items'}++;

	return $resource;
}

1;
