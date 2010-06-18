#!/usr/bin/perl
#
# 辞書作成
#
# Usage:
#  makedic.pl dbpath directory
#

use strict;
use warnings;
use Encode;
use Text::MeCab;
use TokyoCabinet;
use Unicode::Japanese;

sub get_yomi {
    my ($mecab, $word) = @_;
    return if Encode::decode('utf-8', $word)
        !~ m/^[\p{Hiragana}\p{Katakana}\p{Han}]+$/;
    my $yomi_hiragana;
    if (Encode::decode('utf-8', $word) =~ m/^\p{Hiragana}+$/) {
        $yomi_hiragana = $word;
    }
    elsif (Encode::decode('utf-8', $word) =~ m/^\p{Katakana}+$/) {
        my $hiragana = Unicode::Japanese->new($word)->kata2hira; 
        $yomi_hiragana = $hiragana->utf8;
    }
    else {
        my $yomi_all = '';
        for (my $n = $mecab->parse($word); $n; $n = $n->next) {
            my @features = split /,/, $n->feature;
            my $pos = $features[0];
            next if $pos eq 'BOS/EOS';
            my $surface = $features[6];
            my $yomi = $features[7];
            return if !$yomi;
            if ($pos eq '助詞' && $surface =~ /^(の|を|が)$/) {
                print "$word\n";
            }
#            return if $pos eq '助詞' && ($surface eq 'の' || $surface eq 'を');
            $yomi_all .= $yomi;
        }
        my $hiragana = Unicode::Japanese->new($yomi_all)->kata2hira; 
        $yomi_hiragana = $hiragana->utf8;
    }
    $yomi_hiragana =~ tr/ぁぃぅぇぉっゃゅょ/あいうえおつやゆよ/,
    return $yomi_hiragana;
}

my $dbpath = shift @ARGV;
my @textfiles = @ARGV;
if (!$dbpath || !@textfiles) {
    warn "Usage: $0 dbpath txtfile1 txtfile2 ...\n";
    exit 1;
}
my $dicdb = TokyoCabinet::HDB->new();
if (!$dicdb->open($dbpath,
    $dicdb->OWRITER | $dicdb->OCREAT | $dicdb->OTRUNC)) {
    die 'DBM error:' . $dicdb->errmsg($dicdb->ecode);
}
my $mecab = Text::MeCab->new;

foreach my $file (@textfiles) {
    warn "Open text file: $file\n";
    open my $fh, $file;
    if (!$fh) {
        warn "Cannot open $file. Skip it\n";
        next;
    }
    my $title;
    while (my $line = <$fh>) {
        chomp $line;
        if ($line =~ /^\[\[(.+)\]\]$/) {
            $title = $1;
        }
        elsif ($title) {
            my $yomi = get_yomi($mecab, $title);
            next if !$yomi;
            #print "$title\n";
            $title = '';
        }
    }
    close $fh;
}
