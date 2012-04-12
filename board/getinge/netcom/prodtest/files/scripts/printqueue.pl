#!/usr/bin/perl -w
use strict;
use Linux::Inotify2;

#May need to install this package on Ubuntu for the Inotify module:
#  liblinux-inotify2-perl

#Must be full path
my $queue = $ARGV[0];

##################### Setup #######################
# 
# Set up the Inotify2 object, and start polling the
# directory.

my $notify = Linux::Inotify2->new() || die "Couldn't create Inotify2 object: $!";

#Create a new watcher
$notify->watch($queue, IN_CREATE, \&callback);

#Main event loop
while($notify->poll()) {
}

#################### Printing ######################
# 
# Set up a callback function.  When a new file is
# created, we will print it by calling a shell script.
sub callback {
   my $e = shift;
   my $name = $e->fullname;
   my $printer_port;

#   print "$name was created\n";
#   sleep(2);
#   print "Deleting $name\n";
   $printer_port="/dev/usb/lp0" if -e "/dev/usb/lp0";
   $printer_port="/dev/usblp0" if -e "/dev/usblp0" ;
   print localtime() . "  Printing $name to $printer_port\n";
   my $res = `/usr/bin/sudo /bin/cat $name > $printer_port && rm -f $name`;
   print $res
}
