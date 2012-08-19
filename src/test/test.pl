#!/usr/bin/perl

BEGIN {
    system "gcc -o /tmp/wol ../wol.c";
}

use strict;
use warnings;
use TAP::Harness;

my %args = (
    verbosity => 1,
    color => 1
);

my $harness = TAP::Harness->new( \%args );

$harness->runtests(<t/*>);

END {
    system "rm -f /tmp/wol";
}
