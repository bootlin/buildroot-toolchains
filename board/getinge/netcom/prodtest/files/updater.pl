#!/usr/bin/perl -w
use strict;
use LWP::Simple;

my $updatedir = "http://10.1.2.140/getinge/";
my $latesturl = "http://10.1.2.140/getinge/latest.txt";
my $scriptdir = "/opt/getinge/";

#Grace period, for networking (?)
sleep(5);

#Use the 'get' function from LWP::Simple to grab a text file
#with the latest version.  This just has the filename in it, 
#of a form like this:
#  0.01
#
my $new_version = get($latesturl);

#Did we actually download the above?
if($new_version) {
   chomp($new_version);
   #If yes, is the version later than what
   #we already have?
   
   #TODO: Current version in version.conf, which is created
   #if not there.
   my $current_file = `ls $scriptdir | grep '^netcomtest' | grep '.tar.gz'`;
   chomp($current_file);
   $current_file =~ m/netcomtest-(.*)\.tar\.gz/;
   my $current_version = $1;
   
   #If there was no file, set $current_version to 0 and we'll
   #download whatever's on the server.
   if(!$current_file) {
      $current_version = 0;
   }
   
   #Is the new version newer than what we already have?
   if($new_version > $current_version) {
      #Download the new version
      print "New version $new_version greather than $current_version, downlaoding.\n";
      #Just use this variable to catch output, in case we
      #need it late on
      my $output;
      
      #Make the name of the new file.  We don't need the full path,
      #because we'll just use the --directory-prefix option
      my $tar = $updatedir . "netcomtest-" . $new_version . ".tar.gz";
      $output = `/usr/bin/wget --directory-prefix=$scriptdir $tar`;
      
      #Now remove the old one, and untar the new one.
      unlink($scriptdir . "/$current_file");
      #This $tar is like above, but now we want the full path, not the URL.
      $tar = $scriptdir . "netcomtest-" . $new_version . ".tar.gz";
      $output = `/bin/tar -C $scriptdir -xpf $tar`;
      
      #Next we'll write the version number to a file in the script root.
      open(VERS, ">" . $scriptdir . "netcomtest.vers");
      print VERS $new_version;
      close(VERS);
      
   }
   else {
      #If it's not new, don't do anything.
      print "Nothing to update.\n";
      exit 0;
   }
}
else {
   #If no then we're done here, leave things
   #as they were and exit.
   print "No new version could be found.\n";
   exit 0;
}
