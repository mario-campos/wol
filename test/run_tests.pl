#!/usr/bin/perl

BEGIN {
    # compile tests. run `make tests`
}

use strict;
use warnings;
use TAP::Harness;

my $harness = TAP::Harness->new({
    verbosity => 1,
    color => 1,
    exec => ['/bin/sh'],
    errors => 1
});

$harness->runtests(<t/*.t>);
