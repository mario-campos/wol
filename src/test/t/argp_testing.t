#!/usr/bin/perl

use strict;
use warnings;

use Test::Simple tests => 2;

my %tests = (
    # test name => [input, regex expected output]
    '--help' => ['--help', qr/\w/],
    '--version' => ['--version', qr/^\d/]
);

foreach (keys %tests) {
    ok( `/tmp/wol $tests{$_}[0]` =~ $tests{$_}[1], "testing: $_" );
}
