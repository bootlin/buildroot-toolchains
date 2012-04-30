#!/usr/bin/perl -w
use strict;
use File::Path qw(make_path remove_tree);

my $scriptdir = "/opt/getinge/";

# Insert code here that should run before extracting the archive

# addLineToFile line, filename
#  adds line to filename if it's not already present
sub addLineToFile {
  `grep "$_[0]" "$_[1]"`;
  if ($? != 0) {
    `echo "$_[0]" >> "$_[1]"`;
  }
}

#Clean up before applying new version
remove_tree("${scriptdir}prod-def");

#Install needed perl modules
#`cpan Config::Simple` or die "Can't install perl modules: $!\n";


# install needed perl modules as packages
addLineToFile("deb http://debian.pkgs.cpan.org/debian unstable main","/etc/apt/sources.list");
`apt-get update`;
`apt-cache search libtest-harness-perl`;
`apt-get install libconfig-simple-perl`;

exit(0);
