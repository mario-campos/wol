#!/usr/bin/perl -s

# send magic packet to jenny

use strict;
use warnings;
use Net::Wake;

push @_, '78:E7:D1:D9:74:C3' if scalar(@ARGV) == 0 or push @_, @ARGV;

foreach (@_) {
  print "Sending a magic packet to $_\n";
  Net::Wake::by_udp(undef,$_);
}
