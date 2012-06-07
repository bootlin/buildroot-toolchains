#!/usr/bin/perl -w
use strict;
use LWP::Simple;
use File::Path qw(make_path remove_tree);
use File::Copy;

my $updatedir = "http://getingenetcom.com/production/software/v3/";
my $scriptdir = "/opt/getinge/";
my $LOG;
my $useFile=0;  # Set if it is determined that we will download from a file, not http

# Include common functions
do "/opt/getinge/scripts/common_functions.pl";
my $local_conf = get_config("/opt/getinge/config/local.conf");
my %config = %{$local_conf};

sub write_log {
  print $_[0];
  if (fileno(LOG)) {
    print LOG localtime() . " " . $_[0];
  }
}

sub finish {
  write_log("Updater completed ($_[0])\n");  
  close(LOG);
  local( $| ) = ( 1 );
  print "Press <Enter> or <Return> to continue: ";
  my $resp = <STDIN>;
  exit $_[0];
}

sub getVersionFromFile {
  my $result;
  if (! open(FILE, "$_[0]")) {
    write_log "Cound not open $_[0]\n"; 
    finish(1);
  }
  $result = readline( *FILE );
  close(FILE);  
  return $result;
}

# Prepare dl dir
make_path("${scriptdir}dl");

# Prepare/open output file
make_path("/var/netcom");
open(LOG, ">", "/var/netcom/updater.log");
LOG->autoflush(1);

#Check to see if we've overriden the download source for development
if ($config{updateSource}) {
  $updatedir = "$config{updateSource}";
  write_log "Get update from file\n"; 
  $useFile=1;
}

write_log("Updater started - using $updatedir\n");

my $latesturl = $updatedir . "latest.txt";


#Use the 'get' function from LWP::Simple to grab a text file
#with the latest version.  This just has the filename in it, 
#of a form like this:
#  0.01
#
my $retries=0;
my $new_version;

while ($retries < 5) {
  # try to read from developer file first
  if ($useFile eq 1) { 
    $new_version = getVersionFromFile("$latesturl");
  } 
  else {
    $new_version = get($latesturl);
  }
  
  last if ($new_version);  #last is perl's "break"
  sleep 5;
  write_log("Retrying ($retries)\n");
  $retries += 1;
}


#Did we actually download the above?
if($new_version) {
  chomp($new_version);
  #If yes, is the version later than what
  #we already have?

  # Get filename of current version
  my $current_file = `ls ${scriptdir}dl | grep '^netcomtest' | grep '.tar.gz'`;
  chomp($current_file);
  
  # Determine current version
  my $current_version = 0;  # defaults to 0 to ensure that we update if unavailable
  if (-e "netcomtest.vers") {
    write_log("Found version file\n");
    $current_version = getVersionFromFile("netcomtest.vers");
    write_log "current version $current_version.\n";
  }
  else {
    $current_file =~ m/netcomtest-(.*)\.tar\.gz/;
    $current_version = $1;
  }
       
  #Is the new version newer than what we already have?
  if($new_version > $current_version) {
    #Download the new version
    write_log "Downloading new version: $new_version - current version $current_version.\n";
    #Just use this variable to catch output, in case we
    #need it later on
    my $output;

    #The new file
    my $tar = "${scriptdir}dl/" . "netcomtest-" . $new_version . ".tar.gz";
    
    #Make the name of the new file.  We don't need the full path,
    #because we'll just use the --directory-prefix option
    my $src = $updatedir . "netcomtest-" . $new_version . ".tar.gz";
    unless (-e $tar) {
      if ($useFile eq 1) { 
        write_log("Error copying $src:\n") unless copy("$src","${scriptdir}dl");
      }
      else {
        $output = `/usr/bin/wget --directory-prefix=${scriptdir}dl $src`;
      }
    }

    write_log("Done downloading\n");

    # Did we get it?
    unless (-e $tar) {
      write_log("Could not download $src\n");
      finish(1);
    }
        
    #Now remove the old one, and untar the new one.
    unlink("${scriptdir}dl" . "/$current_file") if(-e $current_file);
    
    #Now extract and run the pre-update script
    $output = `/bin/tar -C $scriptdir -xpf $tar ./scripts/preupdate.pl`;
    if ($? != 0) {
      write_log("Could not extract the pre-update script from $tar\n");
      finish(1);    
    }
    
    $output = `scripts/preupdate.pl`;
    if ($? != 0 ) {
      write_log("Could not execute the pre-update script from $tar\n");
      finish(1);    
    }
  
    #This $tar is like above, but now we want the full path, not the URL.
    $output = `/bin/tar -C $scriptdir -xpf $tar`;

    $output = `$scriptdir/scripts/postupdate.pl`;
    if ($? != 0) {
      write_log("Could not execute the post-update script from $tar\n");
      finish(1);    
    }

    #Next we'll write the version number to a file in the script root.
    open(VERS, ">" . $scriptdir . "netcomtest.vers");
    print VERS $new_version;
    close(VERS);
    
    write_log "Updated version from $current_version to $new_version\n";
    finish(0);
  }
  else {
    #If it's not new, don't do anything.
    write_log("Version ($current_version) is up-to-date\n");
    finish(0);
  }
}
else {
   #If no then we're done here, leave things
   #as they were and exit.
   write_log("Could not contact update server\n");
   finish(1);
}
