#!/usr/bin/perl
#
# ナンクロ用の辞書DBMを作成
#
# Usage: 
#  make_dicdb.pl dbpath < textfile
#

use strict;
use warnings;
use Encode qw(decode);
use TokyoCabinet;

sub make_pattern {
    my $word = shift;
    return if !$word;
    my $dec = decode 'utf-8', $word;

    my %map;
    my $cnt = 1;
    my $pattern;
    foreach my $ch (split //, $dec) {
        $map{$ch} = $cnt++ if !exists $map{$ch};
        $pattern .= $map{$ch};
    }
    return $pattern;
}

sub usage_exit {
    print "Usage: make_dicdbm.pl dbpath < textfile\n";
    exit(1);
}

my $dbpath = shift @ARGV;
usage_exit() if !$dbpath;

my $dicdb = TokyoCabinet::HDB->new();
if (!$dicdb->open($dbpath,
    $dicdb->OWRITER | $dicdb->OCREAT | $dicdb->OTRUNC)) {
    die 'DBM error:' . $dicdb->errmsg($dicdb->ecode);
}

while (my $line = <STDIN>) {
    chomp $line;
    next if !$line;
    my $pattern = make_pattern($line);

    my $val = $dicdb->get($pattern);
    if ($val) {
        $val .= " $line";
    }
    else {
        $val = $line;
    }
    $dicdb->put($pattern, $val);
}

if (!$dicdb->close()) {
    die 'DBM error: '.$dicdb->errmsg($dicdb->ecode);
}

