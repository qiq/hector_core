#!/usr/bin/env perl

# This is a Perl stub, that is run for every Perl Module.
use strict;
use Cwd qw(abs_path);

# check command line
if (@ARGV != 1) {
	print STDERR "usage: perl-module.pl module_name\n";
	exit(1);
}
my $module = $ARGV[0];

# get this script directory and adjust Perl library/shared library dirs
my $path = abs_path($0);
$path =~ s/[^\/]*$//;
$ENV{'LD_LIBRARY_PATH'} .= ":$path";
push(@INC, $path);

# get directory of resources

# load module
require $module.".pm";

#use Data::Dumper;
my $instance = $module->new();
$instance->ProcessResource(undef);
