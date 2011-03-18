# TestResource.pm, simple, perl
# Simple test resource implemented in Perl
# 
# Dependencies:
# Parameters:

package TestPerlResource;

use warnings;
use strict;
use Storable;
use Hector;

sub new {
	my ($proto, $resource, $id) = @_;
	my $class = ref($proto) || $proto;
	my $self = {
		'_resource' => $resource,
		'_id' => $id,
		'a' => {
			'type' => "STRING",
			'value' => "",
		},
		'b' => {
			'type' => "HASH_INT",
			'value' => {},
		},
		'c' => {
			'type' => "ARRAY_STRING",
			'value' => [],
		},
	};
	bless($self, $class);
	return $self;
}

sub DESTROY {
}

sub Clear {
	my ($self) = @_;

	$self->{'a'}->{'value'} = "";
	$self->{'b'}->{'value'} = {};
	$self->{'c'}->{'value'} = [];
}

sub Serialize {
	my ($self) = @_;

	my %tmp;
	foreach my $key (keys %{$self}) {
		next if ($key =~ /^_/);
		$tmp{$key} = $self->{$key};
	}

	return freeze(\%tmp);
}

sub Deserialize {
	my ($self, $data) = @_;
	if (not $data =~ s/([^\n]*)\n//) {
		return 0;
	}
	$self->{'_id'} = $1;

	my $s = thaw($data);
	foreach my $key (keys %{$s}) {
		$self->{$key} = $s->{$key};
	}
	return 1;
}

# returns [ [name_1, type_1], [name_2, type_2], ... ]
sub GetAttrInfoList {
	my ($self) = @_;

	my @result;
	foreach my $key (keys %{$self}) {
		next if ($key =~ /^_/);
		push(@result, [ $key, $self->{$key}->{'type'} ]);
	}
	return \@result;
}

sub GetTypeId {
	my ($self) = @_;

	return 3;
}

sub GetTypeString {
	my ($self, $terse) = @_;

	return $terse ? "TPerlR" : "TestPerlResource";
}

sub GetSize {
	my ($self) = @_;

	return 1;
}

sub ToString {
	my ($self) = @_;

	my $s = sprintf("[TPerlR %d %d] ", $self->{'_id'}, $self->{'_resource'}->IsSetFlag($Hector::Resource::DELETED) ? -1 : $self->{'_resource'}->GetStatus());
	return $s;
}

sub Get_a {
	my ($self) = @_;
	return $self->{'a'}->{'value'};
}

sub Set_a {
	my ($self, $value) = @_;
	$self->{'a'}->{'value'} = $value;
}

1;
