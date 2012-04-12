#!/usr/bin/perl -w
use strict;
use IO::Handle;

use Device::SerialPort;
use Time::HiRes qw(usleep nanosleep);

$| = 1; # turn on autoflush

# Include common functions
do "/opt/getinge/scripts/common_functions.pl";
my $conf_ref = get_config("/opt/getinge/config/global.conf");
my %config = %{$conf_ref};

my $testrootdir = $config{testrootdir};

############################################
# 
# Commandline Arguments
#
# Take the following arguments for use in
# testing and generating the reports:
#  *$selected_up  -> Operation(s) to perform
#    op = 1 write bootblock
#    op = 2 serialize
#    op = 4 hw test
#    op = 8 program firmware
#    op = 16 erase bootblock
#
#  *$pos          -> Test position
#  *$_mac         -> MAC address
#  *$_pcb_barcode -> Serial number and more
#  *$prod         -> Product type (A or B)
#  *$user         -> User's initials

my $selected_op   = $ARGV[0]; 
my $pos           = $ARGV[1];
my $_mac          = $ARGV[2];
my $_pcb_barcode  = $ARGV[3];
my $prod          = $ARGV[4];
my $user          = $ARGV[5];

# Globals
my $stateText;  # Holds the current status
my $hwfailText;
my $hwpassText;

print $0;
 foreach (@ARGV) {
 	print " " . $_;
 } 
 	print "\n";

if (@ARGV < 5) {
  print STDERR "Not enough arguments"; 
  exit(1);
}

#Redirect STDERR and STDOUT to a log file.
open(STDERR, ">>" . $testrootdir . "pos" . $pos . "/programmer." . $pos . ".log");
open(STDOUT, ">>" . $testrootdir . "pos" . $pos . "/programmer." . $pos . ".log");

#Make our PID file
write_pid();

# TO DO - load these settings from the appropriate position's config file
my $portName = "/dev/ttyUSB0";

############################################
#
# Functions

# Prints a label, specify 0=fail, 1=pass
sub printLabel {

  # Ensure old files are removed
  unlink ">$testrootdir" . "pos$pos/labelvars";
  unlink ">$testrootdir" . "pos$pos/label.tmp";

   my $res = open(LV, ">$testrootdir" . "pos$pos/labelvars");
   if ( $res ) {
     print LV "_mac=\"$_mac\"\n";
     print LV "_test_location_code=\"unset\"\n";
     print LV "_operator=\"unset\"\n";
     print LV "_swnum=\"unset\"\n";
     print LV "_build=\"unset\"\n";

     print LV "_prod_name=\"unset\"\n";
     print LV "_assy_country=\"unset\"\n";
     print LV "_battery_message=\"unset\"\n";
     print LV "_test_position=\"$pos\"\n";
     print LV "_pcb_barcode=\"$_pcb_barcode\"\n";
     print LV "_pn=\"unset\"\n";
     print LV "_pcb_fact=\"unset\"\n";
     print LV "_pcb_order=\"unset\"\n";
     print LV "_pcb_ser=\"unset\"\n";
     print LV "_pcb_item=\"unset\"\n";
     print LV "_pcb_date=\"unset\"\n";
     print LV "_swver=\"unset\"\n";
     print LV "_test_date=\"unset\"\n";
     print LV "_pcb_fail_text1=\"$hwfailText\"\n";
     print LV "_pcb_fail_text2=\"Please consult test documentation for an explaination of failure codes\"\n";
     print LV "_autoIP=\"unset\"\n";
     print LV "_pcb_rohs=\"unset\"\n";  # [ "`echo $_pcb_barcode | cut -b11`" == "R" ] && _pcb_rohs="RoHS" || _pcb_rohs=""

     close(LV);
   } else {
     print STDERR "print_label:  could not open $testrootdir" . "pos$pos/labelvars\n";
     exit(1);
   }

   my @args = ("/opt/getinge/scripts/zebra.sh", $_[0], "$testrootdir" . "pos$pos/label.tmp", "$testrootdir" . "pos$pos/labelvars");
  if (system(@args) != 0) {
      print STDERR "print_label:  could not execute print script\n";
      exit(1);
    }
}

sub cleanup {
  close (comLog);
  msg(3,"Cleanup done");
}

# This function returns true if there is a request to cancel the test
#  TO DO - the implementation has not been finalized, but at least the function is in place
sub checkCancelled {
  if (-e 'cancelRequest') {
    msg(2,"Cancel requested");
    return 1;
  } else {
    return 0;
  }

}


############################################
#
# Put the main logic here.

unless (-e "$portName") {
  msg(1, "Port $portName does not exist - cannot continue");
  exit;
}

my $portBase = `basename $portName`;

my $port = Device::SerialPort->new($portName); # , 0, "/var/lock/$portBase");
if (!$port) {
  msg(1, "Cannot open port $portName - cannot continue");
  exit;
}

open (comLog, ">$testrootdir" . "pos$pos/comLog.txt");

$port->databits(8);
$port->baudrate(115200);
$port->parity("none");
$port->stopbits(1);

# First we wait for a board to be connected - depending on the state of the
# board, it should either appear as a USB device or just start booting and
# sending data over the debug port.  We should detect either.
# TO-DO - We should ensure that our cables can detect PCB power (using CTS)
#   This allows us to detect when the board is powered on so we can anticpate
#   either a USB device appearance or serial data.  If we get neither we can
#   actually let the user know and maybe even fail the test ie. "no response"

msg(1,"Waiting for boot");
msg(2,"Please apply power to board now.");

my $got_prompt = 0;

my $last_usb_devs = `lsusb -d 03eb:6124 2> /dev/null`;

do {
  if (serWaitRx($port,"AT91Bootstrap",3000)) {
    msg(2,"Got AT91Bootstrap, waiting for U-Boot");
    if (serWaitRx($port,"U-Boot",3000)) {
      msg(3,"Waiting for U-Boot key prompt");
      #if (serWaitRx($port,"stop with ENTER",15000)) {
      if (serWaitRx($port,"In:",5000)) {
        msg(2,"Got U-Boot prompt");
        serTx($port,"\r\n");
        if (serWaitRx($port,"U-Boot>",3000)) {
          if ($selected_op & 0x10) {    # we should erase the dataflash chip and reboot
            msg(2,"Erasing dataflash");
            serTx($port,"mw.l 71000000 AABBCCDD 1080; protect off C0000000 C00041FF;" . 
                        " cp.l 71000000 C0000000 1080;\r\n");
            $port->lookclear;  # flush buffer
            if (serWaitRx($port,"U-Boot>",3000)) {
              msg(2,"Resetting board");
              $port->lookclear;  # flush buffer
              serTx($port,"mw.l FFFFFD00 A500000D;\r\n");
              $selected_op = $selected_op & 0xFFEF;
            }
          } else {
            $got_prompt = 1;
          }
        }
      }
    }
  }

  # Here we detect if an ACM device has been added (completely unprogrammed boards
  #   will show up this way)
  my $usb_devs = `lsusb -d 03eb:6124 2> /dev/null`;
  if ( $last_usb_devs ne $usb_devs ) {
    msg(3,"USB device change detected");
    if ( $usb_devs ne "" ) {
      msg(2,"SAMBA Bootloader Detected");
      msg(3,"$usb_devs");
      # TO DO - Program dataflash at this point
    }
  }
  $last_usb_devs = $usb_devs;

} while (!$got_prompt && !checkCancelled);

# At this point we are at the u-boot prompt, so the first step is to check if a MAC
#   is set and if it matches the one we're attempting to program
  msg(1,"Serializing");
  serTx($port,"printenv ethaddr\r\n");
  my $response = serWaitRx($port,"ethaddr=",3000);
  if ( $response ne 0 ) {
    msg(3,"ethaddr env var exists: $response");
    my $tmpstr = substr($response, 8,17);
    if ( substr($response, 8, 17) ne $_mac ) {
      msg(2,"MAC Address mismatch.  Requested: $_mac  Programmed: $tmpstr");
      # TO DO - React badly here - they're asking you to change the MAC!
    }
  } 
  else # There is no MAC programmed, so program it
  {
    msg(2,"Programming MAC Address");
    serTx($port,"setenv ethaddr $_mac\r\n");
    serWaitRx($port,"U-Boot>",1000);
  }

  # Once the MAC matches, we can program the other vars

  msg(2,"Programming PCB Info");
  serTx($port,"setenv pcbbarcode $_pcb_barcode\r\n");
  serWaitRx($port,"U-Boot>",1000);

  # TO DO - set PCB serial number - actually ORDER + SN fields together
  msg(2,"Saving serialization Info");
  serTx($port,"saveenv\r\n");
  serWaitRx($port,"U-Boot>",1000);

  # And here set vars that should only be valid this session
  serTx($port,"setenv testposition $pos\r\n");
  serWaitRx($port,"U-Boot>",1000);

  # Write the environment to our logs
  serTx($port,"printenv\r\n");
  $port->lookclear;  # flush buffer
  serWaitRx($port,"U-Boot>",10000);

  # Set these too?
  # BOOTBLOCK_PCBSERNO=Z999999
  # BOOTBLOCK_BRAND=GETINGE
  # BOOTBLOCK_HWID=2012
  # BOOTBLOCK_HWCONFIG=1

  msg(1,"Hardware Test");

  # Detect previous hardware test 
  if ( checkHWTestPass($port) == 0 ) {

    # Do not run test again if we have a previous passing test
    #  TO DO - check test version and re-test (or require a re-test)
    #    if old or outdated version is found in result
    msg(2,"No previous passing test results found, starting test");
    # Then start the HW test
    #  TO DO - Make this optional depending on the command line "op"
    serTx($port,"cp.b C0037B00 70000000 6300 && source 70000000;\r\n");

    $got_prompt = 0;
    do {
      $response = serWaitRx($port,"Push TST button",10000);  # This should actually happen twice
      if ($response ne 0) {
        msg(2,"$response");
        $got_prompt ++;
        print(STDOUT "\a"); 
      }

    } while (($got_prompt < 2) && !checkCancelled);
    serWaitRx($port,"HW Test Complete",5000);
    serTx($port,"\r\n");
    serWaitRx($port,"U-Boot>",1000);
    $port->lookclear;  # flush buffer
  }

  # Check for a failed test
  if ( checkHWTestPass($port) == 0 ) {
    printLabel(0);
    msg(2,"Test Failed");
    msg(3,"Fail code: $hwfailText");
  }

  msg(1,"Idle");
cleanup;
exit 0;

print localtime() . "  Test process starting: ";
print_file("Test process starting...\n");

sleep(4);

print "MAC -> $_mac ";
print_file("MAC -> $_mac\n");

sleep(1);

print "SN -> $_pcb_barcode ";
print_file("SN -> $_pcb_barcode\n");

sleep(1);

print_file("Finishing up...\n");

sleep(4);

print "Done!\n";
print_file("Done!\n");

sleep(2);

############################################
#
# Housekeeping

#Get rid of the PID in the file
truncate_file();

#Delete the PID file
write_pid(1);

############################################
#
# Other Functions

# Reads the hwtest results from uboot
# stores results in globals for later use
sub checkHWTestPass {
  my $res = 0;
  my $port = $_[0];

  $hwfailText = "";
  $hwpassText = "";

  msg(2,"Querying previous test results");

  $port->lookclear;  # flush buffer
  serTx($port,"printenv hwtestpass\r\n");
  my $response = serWaitRx($port,"hwtestpass\=",1000);
  if ($response) {
    $response =~ m/.*=(.*)/; #returns everthing after "="
    $hwpassText = $1;
  }

  serTx($port,"printenv hwtestfail\r\n");
  $response = serWaitRx($port,"hwtestfail\=",1000);
  if ($response) {
    $response =~ m/.*=(.*)/; #returns everthing after "="
    $hwfailText = $1;
  }

  msg(3,"Fail: " . $hwfailText);
  msg(3,"Pass: " . $hwpassText);

  if ($hwfailText eq "" && $hwpassText ne "") {
    $res=1;
  } elsif (not $hwfailText eq "" && not $hwpassText eq "") {
    msg(3,"Neither pass/fail valiables populated, assume test never ran.");    
  }
  else { # we have both pass and fail - invalid!
    msg(3,"Invalid state detected - pass and fail valiables populated.");    
  }

  return $res;
}

#Write the pid to a file.  If passed a 1, delete
#said pid file.
sub write_pid {
   my $delete = $_[0];
   
   if($delete) {
      unlink($testrootdir . "pos$pos/pid");
      return 0;
   }
   else {
      my $res = open(PID, ">$testrootdir" . "pos$pos/pid");
     if ( $res ) {
        print PID $$;
        close(PID);
        return 0;
     } else {
       print STDERR "write_pid:  could not open $testrootdir" . "pos$pos/pid\n";
     }
   }
}

#Print something to the status.txt file.  This puts it on
#a new line which can be grabbed by the programNetcom.pl 
#script and put into the appropriate 
sub print_file {
   my $line = $_[0];
   my $res = open(STATUS, ">$testrootdir" . "pos$pos/status.txt");
   if ( $res ) {
     STATUS->autoflush(1);
     print STATUS $line;
     close(STATUS);
   } else {
     print STDERR "print_file:  could not open $testrootdir" . "pos$pos/status.txt\n";
   }
}

#Leave the file, but delete it's contents.
sub truncate_file {
   open(STATUS, ">$testrootdir" . "pos$pos/status.txt");
   truncate(STATUS, 0);
   close(STATUS);
}

#Take the scanner device, and find any other ACM devices
#that are there and are not the scanner device.
sub get_acm {
   my $scanner = $_[0];
   
   my $list = `/usr/bin/find /dev -name ACM*`;
   my @devs = split(/\n/, $list);
}

# msg - this function handles all printing and supports multiple levels
#  usage:  msg level string
#    level 1 - General messages - program states
#    level 2 - Detailed messages - program phases
#    level 3 - Debug data
#  TO DO - direct different levels to different destinations
sub msg {
  my $msgLevel = shift(@_);
  print $_[0] . "\n";
  if ( $msgLevel == 1 ) {  # Changes to test state
    $stateText = $_[0];
    print_file $_[0] . "\n";
  }
  elsif ( $msgLevel == 2) {  # show test state and phase text
    print_file $stateText . " - " . $_[0] . "\n";
  }
}

sub waitMs {
  #debug "Sleeping $_[0] mS\n";
  usleep($_[0] * 1000);
}

sub serTx {
  msg(3,"Sending $_[1]");
  print comLog "< $_[1]\n";
  $_[0]->write($_[1]);
}

# waits on port x to receive string y for up to z mS
sub serWaitRx {
  msg(3,"Waiting for /$_[1]/ for $_[2]mS ");

  my $port = $_[0];

  $port->are_match("\n", "\r");    # possible end strings

  my $endLoop = 0;
  my $count = 0;

  while (($count * 100) < $_[2]) {
    my $gotit = "";
    $gotit = $port->lookfor;       # poll until data ready
    if ($gotit ne "") {
    print comLog "> $gotit\n";
    #print $count . "> $gotit\n";
      if ($gotit =~ /$_[1]/) {
        msg(3,"- found after: " . $count . " mS");
        return $gotit;
      }
    }
    waitMs(100);                          # polling sample time
    $count++;
  }
  msg(3,"- timeout"); 
  return 0;

}



