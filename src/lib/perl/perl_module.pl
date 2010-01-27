#!/usr/bin/env perl

# This is a Perl stub, that is run for every Perl Module.
use strict;
use Cwd qw(abs_path);
use Hector;

# check command line
if (@ARGV == 0) {
	print STDERR "usage: perl-module.pl module_name ...\n";
	exit(1);
}
my @names = @ARGV;
my @modules = ();

# get this script directory and adjust Perl library/shared library dirs
my $path = abs_path($0);
$path =~ s/[^\/]*$//;
$ENV{'LD_LIBRARY_PATH'} .= ":$path:${path}modules";
push(@INC, $path, $path.'modules');

# load modules
foreach my $name (@names) {
	eval "use $name;";
	if ($@) {
		print STDERR "Error loading module: $@;";
		exit(1);
	}
	my $module;
	my $s = '$module = '.$name."->new();";
	eval $s;
	if ($@) {
		print STDERR "Error loading module: $@;";
		exit(1);
	}
	push(@modules, $module);
}

# Connect using stdio
my $connection = Hector::StdioConnection->new();
my $rpc = Hector::RPC->new($connection);
$connection->DISOWN();

my $type = 0;
my $id = 0;
my $s = $rpc->Receive(\$type, \$id);
while (defined $s) {
	if ($type == 0) {
		# RPC message
		my $message = Hector::RPCMessage->new();
		$message->deserialize($s);
		my $method = $message->getMethod();
		my $index = $message->get_module_index();
		my $out = Hector::RPCMessage->new();
		if ($method == $Hector::RPCMESSAGE_INIT) {
			my @args;
			my ($a, $b) = ($message->get_Init(0, 0), $message->get_Init(0, 1));
			my $i = 1;
			while (defined $a) {
				push(@args, [$a, $b]);
				($a, $b) = ($message->get_Init($i, 0), $message->get_Init($i, 1));
				$i++;
			}
			$out->set_response_Init($modules[$index]->Init(\@args));
		} elsif ($method == $Hector::RPCMESSAGE_SAVE_CHECKPOINT) {
			$modules[$index]->SaveCheckpoint($message->get_SaveCheckpoint(0), $message->get_SaveCheckpoint(1));
			$out->set_response_SaveCheckpoint();
		} elsif ($method == $Hector::RPCMESSAGE_RESTORE_CHECKPOINT) {
			$modules[$index]->RestoreCheckpoint($message->get_RestoreCheckpoint(0), $message->get_RestoreCheckpoint(1));
			$out->set_response_RestoreCheckpoint();
		}
		$s = $out->serialize();
		$rpc->Send(0, $id, $s);
	} else {
		# resource: process by all modules
		my $resource = &ConstructResource($type, $s);
		foreach my $m (@modules) {
			$resource = $m->Process($resource);
		}
		$s = $resource->serialize();
		$type = $resource->getType();
		$rpc->Send($type, $id, $s);
	}
	$s = $rpc->Receive(\$type, \$id);
}
