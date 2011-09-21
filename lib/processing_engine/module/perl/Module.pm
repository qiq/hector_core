# Module.pm, base of all Perl Modules

package Module;

use warnings;
use strict;
use Hector;

sub new {
	my ($proto, $object, $id, $threadIndex) = @_;
	my $class = ref($proto) || $proto;
	my $self = {
		'_object' => $object,
		'_id' => $id,
		'_threadIndex' => $threadIndex,
	};
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
		}
	}
	return 1;
}

sub GetType {
	my ($self) = @_;

	return $Hector::Module::SIMPLE;
}

sub Start {
	my ($self) = @_;
}

sub Stop {
	my ($self) = @_;
}

sub Pause {
	my ($self) = @_;
}

sub Resume {
	my ($self) = @_;
}

sub GetValue {
	my ($self, $name) = @_;
	if (exists $self->{$name}) {
		return $self->{$name};
	} else {
		$self->LOG_ERROR("Invalid value name: $name");
		return undef;
	}
}

sub SetValue {
	my ($self, $name, $value) = @_;
	if (exists $self->{$name}) {
		$self->{$name} = $value;
	} else {
		$self->LOG_ERROR("Invalid value name: $name");
		return 0;
	}
	return 1;
}

sub ListNames {
	my ($self) = @_;
	return [ grep { $_ !~ /^_/ } keys %{$self} ];
}

sub SaveCheckpoint {
	my ($self, $path, $id) = @_;
	$self->LOG_INFO("SaveCheckpoint($path, $id)");
}

sub RestoreCheckpoint {
	my ($self, $path, $id) = @_;
	$self->LOG_INFO("RestoreCheckpoint($path, $id)");
}

sub ProcessSimple() {
	my ($self, $resource) = @_;

	return $resource;
}

sub LOG_TRACE() {
	my ($self, $resource, $message) = @_;
	if (defined $message) {
		$message = ": ".$resource->ToStringShort()." ".$message;
	} else {
		$message = $resource;
	}
	$self->{'_object'}->log_trace($message);
}

sub LOG_DEBUG() {
	my ($self, $resource, $message) = @_;
	if (defined $message) {
		$message = ": ".$resource->ToStringShort()." ".$message;
	} else {
		$message = $resource;
	}
	$self->{'_object'}->log_debug($message);
}

sub LOG_INFO() {
	my ($self, $resource, $message) = @_;
	if (defined $message) {
		$message = ": ".$resource->ToStringShort()." ".$message;
	} else {
		$message = $resource;
	}
	$self->{'_object'}->log_info($message);
}

sub LOG_WARN() {
	my ($self, $resource, $message) = @_;
	if (defined $message) {
		$message = ": ".$resource->ToStringShort()." ".$message;
	} else {
		$message = $resource;
	}
	$self->{'_object'}->log_warn($message);
}

sub LOG_ERROR() {
	my ($self, $resource, $message) = @_;
	if (defined $message) {
		$message = ": ".$resource->ToStringShort()." ".$message;
	} else {
		$message = $resource;
	}
	$self->{'_object'}->log_error($message);
}

1;
