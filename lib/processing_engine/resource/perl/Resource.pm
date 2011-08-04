# Resource.pm, base of all Perl Resources

package Resource;

use warnings;
use strict;
use Hector;

sub new {
	my ($proto, $resource, $id) = @_;
	my $class = ref($proto) || $proto;
	my $self = {
		'_resource' => $resource,
		'_id' => $id,
	};
	bless($self, $class);
	return $self;
}

sub DESTROY {
}

sub Clear {
	my ($self) = @_;
}

sub Serialize {
	my ($self) = @_;

	return undef;
}

sub Deserialize {
	my ($self, $data) = @_;

	return 0;
}

sub GetSize {
	my ($self) = @_;

	return 1;
}

sub GetResourceInfo {
	my ($self) = @_;

	return undef;
}

sub ToString {
	my ($self) = @_;

	return "";
}

1;
