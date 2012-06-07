#!/usr/bin/perl -w
use strict;
use Config::Simple;
use Time::HiRes qw(gettimeofday tv_interval usleep);
use Device::SerialPort;

my $local_conf = new Config::Simple("/opt/getinge/config/local.conf");

# Start a timer to be used for timestamps
my $appStartTime = [gettimeofday];

# Globals
my $port;

# Returns the time since the program was started
sub getElapsedTime {
  return sprintf('%.3f', tv_interval($appStartTime));  
}

# msg - this function handles all printing and supports multiple levels
#  usage:  msg level string
#    level 1 - General messages - program states
#    level 2 - Detailed messages - program phases
#    level 3 - Debug data
#  TO DO - direct different levels to different destinations
sub msg {
  my $msgLevel = shift(@_);
  print getElapsedTime() . ": " . $_[0] . "\n";
}

sub waitMs {
  #debug "Sleeping $_[0] mS\n";
  usleep($_[0] * 1000);
}

sub serTx {
  #msg(3,"Sending $_[1]");
  print getElapsedTime() . " < $_[1]\n";
  $_[0]->write($_[1]);
  $_[0]->write_drain;
}

# waits on port x to receive string y for up to z mS
sub serWaitRx {
  msg(3,"Waiting for /$_[1]/ for $_[2]mS ");

  my $port = $_[0];

  my $endLoop = 0;
  my $count = 0;

  while (($count * 10) < $_[2]) {
    my $gotit = "";
    $gotit = $port->lookfor;       # poll until data ready
    if ($gotit ne "") {
    print getElapsedTime() . " > $gotit\n";
    #print $count . "> $gotit\n";
      if ($gotit =~ /$_[1]/) {
        msg(3,"- found after: " . $count * 10 . " mS");
        return $gotit;
      }
    }
    waitMs(10);                          # polling sample time
    $count++;
  }
  msg(3,"- timeout"); 
  return 0;

}

# returns all lines received on port x in y ms
sub serTimedRx{
  $port->are_match("\n", "\r");    # possible end strings

  my $inData = "";
  my $tmpData;
  my $count = 0;
  
  while (($count * 10) < $_[1]) {
    $tmpData = $_[0]->lookfor;
    if ($tmpData ne "") {
      $inData = $inData . $tmpData . "\r\n";
    }
    waitMs(10);                          # polling sample time
    $count++;
  }
  
  return $inData;
}

################################################################################
#
#  Begin main program

my $portName;


#( defined("$portName" ) && 

unless ($portName = $local_conf->param('switchPort')) {
  msg(1, "switchPort not configured - cannot continue");
  exit;
}

unless (-e "$portName") {
  msg(1, "Port $portName does not exist - cannot continue");
  exit;
}

my $portBase = `basename $portName`;

$port = Device::SerialPort->new($portName); # , 0, "/var/lock/$portBase");
if (!$port) {
  msg(1, "Cannot open port $portName - cannot continue");
  exit;
}

$port->databits(8);
$port->baudrate(115200);
$port->parity("none");
$port->stopbits(1);
$port->handshake("none");
$port->write_settings;
$port->are_match("\n", "\r", ":", "?");

my $inString;

waitMs(200);

# try to get to the prompt
do {

  # wait for any communication
  do {
    serTx($port,"\r");
    $inString = serTimedRx($port,1000);
  } while ( $inString eq "");
  
  # only log in if we have to
  if ( $inString =~ /User Name:/ ) {
    # For some reason, i could not get serWaitRx to work during login...
    #  for now we'll have to just assume that this will log us in
    #  and no damage will be done if we're already logged in
    serTx($port,"cisco\r");
    waitMs(200);
    serWaitRx($port,"Password:",500);
    serTx($port,"cisco\r");
    waitMs(200);
    
    # we may be prompted to change the password - answer no
    if (serWaitRx($port,"change the password",1500)) {
      serTx($port,"N\r");    
    }
  } elsif ( $inString =~ /Password:/ ) {  # we're out of sync
      serTx($port,"\r");        
  }
} until ( $inString =~ /switch/ );

$port->are_match("\n", "\r", "#");
# and be sure that we are not in config mode - indicated by (parenthesis)
while ( $inString =~ /\(.*\)/ ) {
  $port->lookclear;  # flush buffer
  serTx($port,"exit\r");
  $inString = serWaitRx($port,"switch",500);
}

$port->are_match("\n", "\r");    # possible end strings

# now verify that we're logged in
#serTx($port,"\r");  
#if (serWaitRx($port,"switch",1500)) {
#  print "Logged in.\r\n";
#  # TO DO - if we're not logged in and at the normal prompt, we have a problem!
#}

#cisco switch data
#
#term datadump
#
#show power inline consumption FastEthernet 1
#
#conf
#interface FastEthernet 1
#power inline auto
#power inline never
#
#clear counters FastEthernet 1
#show interface counters FastEthernet 1
#
#test cable-diagnostics tdr interface FastEthernet 1
#  Cable on port fa1 is open at 0 m
#  Cable on port fa2 is open at 2 m
#  Cable on port fa2 is good
#



do {
  $port->are_match("#");

  $port->lookclear;  # flush buffer
  serTx($port,"show power inline consumption\r");  
  #print serTimedRx($port,500);
  print serWaitRx($port,"switch",2000);

  $port->lookclear;  # flush buffer
  serTx($port,"show interface counters FastEthernet 1\r");  
  print serTimedRx($port,1000);
  #print serWaitRx($port,"switch",2000);

  serTx($port,"config\r");  
  serWaitRx($port,"switch",250);
  
  $port->lookclear;  # flush buffer
  serTx($port,"test cable-diagnostics tdr interface FastEthernet 1\r");  
  #print serWaitRx($port,"switch",2000);
  print serTimedRx($port,1000);
  
  # TO DO - check each loop that we're still acting normal
  serTx($port,"\r");  
  serWaitRx($port,"switch",250);
}
while (0);

serTx($port,"config\r");
waitMs(200);
serTx($port,"int FE 1\r");

exit;


