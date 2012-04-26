#!/usr/bin/perl -w
use strict;
use File::Path qw(make_path remove_tree);

my $scriptdir = "/opt/getinge/";

# Insert code here that should run before extracting the archive

#Clean up before applying new version
remove_tree("${scriptdir}prod-def");

#Install needed perl modules
`cpan Config::Simple` or die "Can't install perl modules: $!\n";

exit(0);
