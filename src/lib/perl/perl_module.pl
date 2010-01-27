#!/usr/bin/env perl

# This is a Perl stub, that is run for every Perl Module.
use strict;
use Cwd qw(abs_path);
use RPCMessage;
use RPC;
use Connection;
use StdioConnection;

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
my $connection = StdioConnection::StdioConnection->new();
my $rpc = RPC::RPC->new($connection);

print Dumper($rpc);

# Dispatch requests
