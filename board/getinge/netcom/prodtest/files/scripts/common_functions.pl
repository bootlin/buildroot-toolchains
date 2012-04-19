#!/usr/bin/perl -w
use strict;

# Common functions.
# If it could conceivably be used across multiple scripts,
# it can go in here.  This file can be included in other
# scripts by doing something like this:
#
#     do "./common_functions.pl";
#
# That of course assumes that the script is in the same
# directory as this file.  Otherwise, use the full path
# to this file.

#Get the config file parsed, return a hash reference.  This
#can also work on product definition files.
sub get_config {
   my $file = $_[0];
   my %conf;

   #open(FILE, $testrootdir . "/pos" . $pos . "$_[0]");
   open(FILE, $file);
   
   while(<FILE>) {
      chomp($_);
      #See http://inthebox.webmin.com/one-config-file-to-rule-them-all
      s/#.*//; # Remove comments
      s/^\s+//; # Remove opening whitespace
      s/\s+$//;  # Remove closing whitespace
      next unless length;
      my @param = split(/=/, $_);
      $conf{$param[0]} = $param[1];
   }

   close(FILE);
   return \%conf;
}

# Counts files in specified directory
sub count_files {
   my $dir = $_[0];
   opendir(PROD, $dir);
   #Exclude . and ..
   my @files = grep(!/^\.\.?$/, readdir(PROD));
   closedir(PROD);
   return scalar(@files);
}

#Get the list of products available.  Take a directory as
#an argument, and return an array.  Products are in the 
#convention of productname.conf so we can tell just by
#listing the directory.
sub get_products {
   #We need to have a trailing slash on the end of the dir
   my $dir = $_[0];
   opendir(PROD, $dir);
   #Exclude . and ..
   my @files = grep(!/^\.\.?$/, readdir(PROD));
   closedir(PROD);
   foreach(@files) {
      #Remove the .conf extension, leaving the name
      $_ =~ s/.conf//;
   }
   return @files;
}

#Get the current version of the test system, given a version
#file.
sub get_version {
   my $vers_file = $_[0];
   my $version;
   open(VERS, "$vers_file");
   local $/ = undef;
   $version = <VERS>;
   close(VERS);
   return $version;
}
   
1;
