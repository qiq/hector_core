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
use Module;
our @ISA = qw(Module);

sub new {
	my ($proto, $object, $id, $threadIndex) = @_;
	my $class = ref($proto) || $proto;
	my $self = $class->SUPER::new($object, $id, $threadIndex);

	$self->{'_resources'} = [];
	$self->{'items'} = 0;
	$self->{'foo'} = 0;
	$self->{'timeTick'} = 1000;

	bless($self, $class);
	return $self;
}

sub DESTROY {
}

sub GetType {
	my ($self) = @_;
	return $Hector::Module::MULTI;
}

my $MAX_RESOURCES = 100;
sub ProcessMulti() {
	my ($self, $inputResources, $outputResources) = @_;

	return (0, 0, $MAX_RESOURCES) if (not defined $inputResources or not defined $outputResources);

	while (@{$inputResources} > 0 and @{$self->{'_resources'}} <= $MAX_RESOURCES) {
		my $resource = shift(@{$inputResources});
		if ($resource->GetTypeString() eq 'TestResource') {
			push(@{$self->{'_resources'}}, $resource);
		} else {
			push(@{$outputResources}, $resource);
		}
	}

	return (0, 0, $MAX_RESOURCES) if (@{$self->{'_resources'}} == 0);

	select(undef, undef, undef, $self->{'timeTick'}/1000000);

	my $resource = shift(@{$self->{'_resources'}});
	push(@{$outputResources}, $resource);
	$self->LOG_INFO($resource, "Processed (".$resource->GetStr().")");
	$self->{'items'}++;

	return (@{$self->{'_resources'}} > 0, $MAX_RESOURCES-@{$self->{'_resources'}}, scalar @{$self->{'_resources'}});
}

1;
