#!/usr/bin/perl

use strict;
use ProtoBuf::Resource;
use Data::Dumper;

sub readBytes() {
	my ($len) = @_;
	my $buffer = "";
	my $read_bytes = 0;
	while ($read_bytes < $len) {
		my $s;
		my $r = sysread(STDIN, $s, $len-$read_bytes);
		if ($r <= 0) {
			print STDERR "Cannot read from input\n";
			return "";
		}
		if ($r > 0) {
			$buffer .= $s;
			$read_bytes += $r;
		}
	}
	return $buffer;
}

sub writeBytes() {
	my ($buffer) = @_;
	my $len = length($buffer);
	my $written_bytes = 0;
	while ($written_bytes < $len) {
		my $w = syswrite(STDOUT, $buffer, $len-$written_bytes, $written_bytes);
		if ($w <= 0) {
			print STDERR "Cannot write to output\n";
			return 0;
		}
		if ($w > 0) {
			$written_bytes += $w;
		}
	}
	return $written_bytes;
}

sub bytes2int() {
	my ($str) = @_;
	my $result = 0;
        for (my $i = 0; $i < 4; $i++) {
                $result = ($result << 8) | ord(substr($str, 3-$i, 1));
        }
        return $result;
}

sub int2bytes() {
	my ($n) = @_;
	my $str = "XXXX";
        for (my $i = 0; $i < 4; $i++) {
		substr($str, $i, 1) = chr($n & 0xFF);
                $n >>= 8;
        }
	return $str;
}

sub Process() {
	my ($resource) = @_;
	print STDERR "Processing: ".$resource->url()."\n";
	$resource->set_url($resource->url()."&OK");
	return $resource;
}

while (1) {
	my $a = &readBytes(4);
	if (length($a) != 4) {
		print STDERR "Cannot read 4 bytes\n";
		exit;
	}
	my $len = &bytes2int($a);
	my $sin = &readBytes($len);
	if (length($sin) != $len) {
		print STDERR "Cannot read message\n";
		exit;
	}

	my $r = ProtoBuf::Resource::Resource->new;
	$r->parse_from_string($sin);
	$r = &Process($r);
	my $sout = $r->serialize_to_string();

	$len = length($sout);
	my $b = &int2bytes($len);
	my $w = &writeBytes($b);
	if ($w != 4) {
		print STDERR "Cannot write 4 bytes\n";
		exit;
	}
	my $w = &writeBytes($sout);
	if ($w != $len) {
		print STDERR "Cannot write message\n";
		exit;
	}
}
