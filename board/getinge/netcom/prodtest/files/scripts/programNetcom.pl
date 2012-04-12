#!/usr/bin/perl -w
use strict;
use Curses::UI;
use IO::Handle;
use Archive::Zip qw( :ERROR_CODES :CONSTANTS );

#May need to install libcurses-ui-perl in Ubuntu
#See http://search.cpan.org/~mdxi/Curses-UI-0.9609/ for more about this
#module.
#
use Device::SerialPort;
use feature 'state';

#Import some functions we'll need
do "/opt/getinge/scripts/common_functions.pl";
my $conf_ref = get_config("/opt/getinge/config/global.conf");

#Get the config hash.  This is taken from the global.conf file,
#but we'll add some other things to it later, like initials.
my %config = %{$conf_ref};

#Get the version.  This will be used in the title of the main window.
$config{version} = get_version($config{scriptrootdir} . "netcomtest.vers");

##### Configuration #####
#How many boards do we want to monitor at most?
my $boards = 8;

#Arrays to contain board information
my @board_windows;
my @board_text;

#Window positioning
my ($height, $width) = (6,40);
my ($xoffset, $yoffset) = (1, 1);

#Test file root directory (full path)
#We need to be able to write to this
#my $testrootdir = "/var/netcom/";
my $testrootdir = $config{testrootdir};

#Where are the scripts going to be located?
#Change to "/opt/getinge/" for live environment
#my $scriptpath = "./";
my $scriptpath = $config{scriptrootdir};

#Executable scripts to be called
my $programmer = $scriptpath . "scripts/runtest.pl";
my $printer = $scriptpath . "scripts/printqueue.pl";

#Barcode scanner vid
my $scanner_vid = "05e0";

#Terminal log file
my $term_log = "test.log";

#Command hash table (For commandline, because humans prefer words to numbers there)
my %cmd = (
            cancel => '2001',
            print => '2002',
            send => '2003',
            install_replace => '2004',
            starttest => '2005',
            reboot => '2006',
            poweroff => '2007',
            test => '2008',
            erase => '2009',
         );

##### Initialization Functions #####
#
# Set everything up before going into the main loop.

# Redirect STDERR to a log file.
open(STDERR, ">>$testrootdir/ui-error.log");

# Find out which serial port has the barcode scanner.
sub scanner_init {
   #Take a vendor ID for the scanner
   my $ids = $_[0];

   my $paths = `/bin/grep $ids \`find /sys/ -name idVendor\``;
   
   $paths =~ s/idVendor:$ids//;
   chomp($paths);
   my $acm_path = `/usr/bin/find $paths -name ttyACM\*`;
   chomp($acm_path);
   $acm_path =~ m/\.*\/ttyACM(\d)/;
   if(defined($1)) {
      return "/dev/ttyACM$1";
   }
   else {
      return 0;
   }
}

#Check for the scanner and setup the device if it gets plugged in.
sub scanner_check {
   my $scanner;
   my $scanner_dev;
   if($scanner_dev = scanner_init($scanner_vid)) {
      $scanner = Device::SerialPort->new($scanner_dev, "false");
      $scanner->databits(8);
      $scanner->baudrate(115200);
      $scanner->parity("none");
      $scanner->stopbits(1);
      $scanner->are_match("\n", "\r");
   }
   return $scanner;
}
  
# Return all of the USB serial devices
sub check_tty {
   my $tty = `/bin/ls /dev | /bin/grep ttyUSB`;
   
   my @devs = split(/\n/, $tty);
   foreach(@devs) {
      $_ = "/dev/" . $_;
   }
   return @devs;
}
############## Initialization ##############

# Command stack for building commands
my @command;

if(! -e $testrootdir ) {
   print "Creating folder " . $testrootdir . "\n";
   mkdir($testrootdir);
}

# Check for the directories we need, create them if they don't exist
for(my $i = 1; $i <= $boards; $i++) {
   if(! -e $testrootdir . "pos" . $i) {
      print "Creating folder " . $testrootdir . "pos" . $i . "\n";
      mkdir($testrootdir . "pos" . $i);
      chmod(0777, $testrootdir . "pos" . $i);
      my $cmd = $testrootdir . "pos" . $i . "/status.txt";
      my $bash = `touch $cmd`;
   }
}

if(! -e $testrootdir . "print") {
   print "Creating folder " . $testrootdir . "print/\n";
   mkdir($testrootdir . "print");
}

if(! -e $testrootdir . "samba-output") {
   print "Creating folder " . $testrootdir . "samba-output/\n";
   mkdir($testrootdir . "samba-output");
}

if(! -e $testrootdir . "samba-status.txt") {
   print "Creating " . $testrootdir . "samba-status.txt";
   my $cmd = $testrootdir . "samba-status.txt";
   my $bash = `touch $cmd`;
}

# Get scanner device and open port
my $scanner = scanner_check();

#if($scanner_dev = scanner_init($scanner_vid)) {
#   $scanner = Device::SerialPort->new($scanner_dev, "false");
#   $scanner->databits(8);
#   $scanner->baudrate(115200);
#   $scanner->parity("none");
#   $scanner->stopbits(1);
#   $scanner->are_match("\n", "\r");
#}

#Start the print queue
fork_cmd($printer, "/var/netcom/print");

#Start Sam-ba
fork_cmd("/opt/getinge/scripts/start_sam-ba.sh");

# Setup the UI
#Main Curses UI object
my $cui = new Curses::UI( -color_support => 1 );

# Ask the user for their initials
while(!$config{initials}) {
   $config{initials} = $cui->question('Please type your intials and then Enter.');
   sleep(0.1);
}

if (0) {  # Since we now run in x, we expect that the keyboard is configured there.
	# Now ask for the keyboard layout
	while(!$config{layout}) {
	   $config{layout} = $cui->question('Choose a keyboard layout.  Enter 1 for Danish, 2 for US.');
	   sleep(0.1);
	}

	#Set the layout
	my $loadkeys;
	if($config{layout} == 2) {
	   $loadkeys = `/usr/bin/sudo /usr/bin/loadkeys /usr/share/keymaps/i386/qwerty/us.kmap.gz`;
	}
	else {
	   $loadkeys = `/usr/bin/sudo /usr/bin/loadkeys /usr/share/keymaps/i386/qwerty/dk.kmap.gz`;
	}
}

##### UI #####
#Main Window
my $main_win = $cui->add(
   'win', 'Window',
   -border => 1,
   -bfg => 'red',
   -title => 'NetCom Programmer Interface v' . $config{version},
);


#Set a binding for the Exit dialog so we can exit
$cui->set_binding( \&quit, "\cQ");
$cui->set_binding( \&update, "\cY");
 
# Prompt for product
$config{product} = promptForProduct($config{product});
            
#Populate the window arrays
for (my $i = 0; $i < $boards; $i++) {
   my ($x, $y);
   #Left Column
   if($i % 2 == 0) {
      $x = 0 + $xoffset;
      $y = (($i/2) * $height) + $yoffset;
   }
   #Right Column
   else {
      $x = $width + $xoffset;
      $y = $y = ((($i-1) / 2) * $height) + $yoffset;
   }
   
   my $window = $main_win->add(
      "win".$i."", 'Window',
      -border => 1,
      -bfg => 'red',
      -title => "Board " . ($i+1) . " Status",
      -height => $height,
      -width => $width,
      -x => $x,
      -y => $y,
   );
   
   push(@board_windows, $window);
}

#Populate each of the windows from above with TextViewers
for(my $i = 0; $i < $boards; $i++) {
   $board_text[$i] = $board_windows[$i]->add(
    "board". $i ."view", 'TextEditor', -focusable => 0, -readonly => 1, -wrapping => 1,
   );  
}

#TextEntry for adding some text
my $shell_window = $main_win->add(
   'shell_win', 'Window',
   -border => 1,
   -bfg => 'red',
   -title => 'Command Line',
   -height => 12,
   -width => 2 * $width,
   -x => $xoffset,
   -y => $yoffset + $height * ($boards / 2),
   );

my $cmd_window = $shell_window->add(
   'cmd_win', 'Window',
   -border => 1,
   -bfg => 'green',
   -width => 2 * $width - 1,
   -height => 2,
   -y => 7,
   );

my $cmdline = $cmd_window->add(
   'cmdline', 'TextEditor',
   -wrapping => 1,
   );

my $output_window = $shell_window->add(
   'output_win', 'Window',
   -border => 1,
   -bfg => 'green',
   -width => 2 * $width -1,
   -height => 8,
   );

my $output_box = $output_window->add(
   'output', 'TextEditor',
   -readonly => 1,
   );

#Status window
my $status_window = $main_win->add(
   'status_win', 'Window',
   -border => 1,
   -bfg => 'blue',
   -title => "System Info",
   -width => 50,
   -height => 20,
   -y => $yoffset,
   -x => 2 * $width + 3,
   );

my $status_box = $status_window->add(
   'status', 'TextEditor',
   -readonly => 1,
   -wrapping => 1,
   );

#Put some lines in the above window
info_box("Operator", $config{initials});
info_box("Product", $config{product});
info_box("Sam-ba Status", "");

#Exit dialog box
sub exit_dialog() {
   my $return = $cui->dialog(
      -message => "Do you really want to quit?",
      -title   => "Are you sure?",
      -buttons => ['yes', 'no'],
   );

   if($return) {
      exit(0);
   }
}

sub quit() {
   kill_stuff();
   exit(0);
}

##############################################################
###################### Main Program Flow #####################

#Set up a main loop with a callback, so we can do other things
#$cui->add_callback("callback", \&callback);
#$cui->set_timer('timer', \&callback);
#$cui->enable_timer('timer');
$cmdline->focus();

$cui->{-read_timeout} = 0.1;

#set_focus(0);
#$cui->do_one_event();
while(1) {
   handle_scanner();       #If there's data, do something
   handle_terminal();      #If there's a command on the terminal, do something
   update_boxes();         #Look in test logs, update each box
   $cui->do_one_event();   #Update the GUI
   sleep(0.1);
}

    

##############################################################
######################### UI Functions #######################

# Call this function with an optional default product value
sub promptForProduct {
  
  use Curses qw(KEY_ENTER);
  use Curses::UI::Common;
  use Curses::UI::Window;

  #Get product and ask to select one.
  my @products = get_products("/opt/getinge/prod-def/");
  #Make an array of hashes for each product, so we have a button label
  #and a value
  my @product_buttons;
  my $prod_count;
  my $selected_prod = 0;
  my $default_prod = $_[0] || ''; 
  foreach(@products) {
    my $rec = {};
    if ( $default_prod eq $_ ) {
      $selected_prod = $prod_count;   
    }
    $prod_count += 1;
    $rec->{-label} = $prod_count . '. ' . $_ . ' - ' . 'Description';
    $rec->{-value} = $_;
    $rec->{-shortcut} = $prod_count;
    push(@product_buttons, $rec);
  }

  my $container = $main_win->add('prod_window', 'Window',
      -border      => 1,
      -width       => 40,
      -centered => 1,
      -height       => 5 + $prod_count
  );  
  
  my $text = $container->add('message', 'TextViewer',
      -wrapping    => 1,
      -padleft   => 1,
      -padright   => 1,
      -padbottom   => 2,
      -text        => 'Select Product',
  -focusable   => 0,
  );  
  
  my $buttons = $container->add(
      'mybuttons', 'Buttonbox',
      -buttons   => \@product_buttons,
      -x   => 1,
      -y   => 2,
      -vertical   => 1,
      -selected => $selected_prod
  );    

  my $button_width = 0;
  if (defined $buttons) {
      $button_width = $buttons->compute_buttonwidth;
  }    
  $container->{-width}=$button_width+10;
  $container->draw;

  my $selection = '';
  
  my $make_select = sub {
    my $this = shift;
    $selection = $this->get; 
    $this->loose_focus();
  };
  $buttons->set_binding( $make_select, KEY_ENTER());
  $buttons->modalfocus;

  $container->parent->delete('prod_window');
  return $selection;
}
  
############# Handler Functions #############
#
# These functions are checked in the main loop
# above, and handle certain actions.  There are
# a few main thing we are concerned with, other
# than updating the GUI:
#  *  Scanner commands
#  *  Terminal commands
#  *  Update status boxes
#

#Update a box, just to test
sub update {
   my $old = $output_box->text();
   my $time = time();
   #$output_box->text($old . "\n" . $time);
   print_console($time);
}

#Check the scanner device.  
sub handle_scanner {
   if($scanner) {
      my $string = $scanner->lookfor();
      if(!$string) {
         #If there's nothing, just return
         return 0;
      }
      do_cmd($string);
   }
   else {
      #If there's no object set up, check for
      #a device and set it up if it's there.
      if($scanner = scanner_check()) {
         print_console("Scanner connected");
      }
   }
}

#Check the 'terminal' TextEditor box and take
#appropriate action.
sub handle_terminal {
   my $input = $cmdline->text();
   if($input !~ m/\n$/) {
      return 0;
   }
   chomp($input);
   my @inarray = split(/ /, $input);
   #Commands entered here ultimately go to the same place
   #as they do with the scanner (ie, the do_cmd() function).
   #However we have this preliminary step before we send it out.
   #We can enter a bunch of parameters in a row, as opposed
   #to scanning them one at a time, so first we split based on
   #a single whitespace character.  (That is, / / in the above.)
   foreach(@inarray) {
      #Is this a key in the %cmd hash?  If so, get the number
      #and submit that.
      if(exists($cmd{$_})) {
         do_cmd($cmd{$_});
      }
      #Otherwise, is it 8, 12, or 35 characters long?  If so, 
      #submit it.
      elsif($_ =~ s/(.)/$1/sg == 8 || $_ =~ s/(.)/$1/sg == 12 || $_ =~ s/(.)/$1/sg > 20) {
         do_cmd($_);
      }
      #Otherwise, let's just ignore it for now.
#      else {
#         print_console($_);
#      }
   }
   #print_console($cmd{"starttest"});
   $cmdline->text("");
}

#Look in $testdir and update the status boxes
#in the GUI.
sub update_boxes {
   for(my $i = 1; $i <= $boards; $i++) {
      my $status = $testrootdir . "pos$i/status.txt";
      if(-e $status) {
         #This next line lets us slurp the file into one string
         local $/=undef;
         open(STATUS, "<$status");
         STATUS->autoflush(1);
         my $status_msg = <STATUS>;
         close(STATUS);
         if($status_msg) {
            #$board_text[$i]->text($status_msg);
            box_text("", $status_msg, $i - 1);
         }
         else {
            #$board_text[$i]->text("Idle");
            box_text("", "Idle", $i - 1);
         }
      }
   }
   local $/=undef;
   open(STATUS, "<" . $testrootdir . "samba-status.txt");
   my $samba_status = <STATUS>;
   close(STATUS);
   chomp($samba_status);
   info_box("Sam-ba Status", $samba_status);
   return 0;
}

############# Helper Functions #############
#
#

#Build up a command in the @command array using either
#the scanner or the commandline (or both?).  After something
#is added to this array, check it here and take action if
#need be.
sub do_cmd {
   my $count;
   my $pos;          #Temp variable for parsing postion from scanner
   my $line = $_[0]; #Input from scanner
   my $focus;        #Last position scanned
   my $selected_op = 0;

   #If the line we get passed is a test position, we reset the focus
   #to that position.  Otherwise, push it onto @command.
   if($line =~ s/(.)/$1/sg == 8 && $line < 1100) {
      $pos = $line % 1000;
      $focus = 1 + set_focus($pos - 1);
      print_console("Position $pos, focus $focus");
      #box_text("", "", $focus-1);
   }
   else {
      $focus = 1 + set_focus();
      print_console("$line");
      push(@command, $line);
   }
   
   #Note:  The $focus variable refers to the test positions as they
   #appear, from 1 to 8 as opposed to 0 to 7 (which is how we refer
   #to them in the internal data structures).  To correct for this,
   #subtract 1 when using an array or passing to a function, and
   #add 1 when we want to get to this from an internal reference.
   
   ########################## Command Parser ##########################
   #
   #This is the main flow for executing different commands.  Basically,
   #we have the last test position scanned in the $focus variable, and
   #then we can use whatever is in the @command array to determine what
   #to do.  One way to validate things (ie, do we have a MAC or a serial?)
   #is to count the characters.  Use this regex:
   #
   #     $string =~ s/(.)/$1/sg == 12
   #
   #In this case, we're checking the length of $string.  This whole thing
   #will return true if it has 12 characters, so we can do something like
   #this:
   #
   #     if($string =~ s/(.)/$1/sg == 12) {
   #        do_stuff();
   #     }
   #
   #This is important when we want to check for something like a MAC (12
   #chars) or a serial (35 chars).  If we don't have to do this, though,
   #we can use a normal numerical comparison.  For example:
   #
   #     if($string == 2001) {
   #        cancel();
   #     }
   #
   #Now, we can build up the commands as follows:
   #
   #  *For each condition, print something to the console using
   #   the print_console() function, which takes two arguments.
   #   The first is the string, while the second (optional) is simply
   #   a 1 if we want to append the string to the previous line.
   #
   #  *Take whatever action we were going to do.  This could be as simple
   #   as getting a PID and issuing a kill command, or we could fork a
   #   new process (see below).
   #
   #  *Clear the @command array by setting it to ().  If we actually
   #   have a command, by this point we're done with it, and now we
   #   can wait for a new one to build up.
   #
   #TODO:   Set up a has to map between English words and commands,
   #        so we can reuse this function for input from the keyboard.
   #
   
   #First, check to see if there is a '0' position in the array
   if(defined($command[0])) {
      
      #Now, let's see what it is and take the appropriate action
      if(($command[0] == 2005) || ($command[0] == 2009) ){
      
         if(defined($command[1]) && $command[1] =~ s/(.)/$1/sg == 12) {
            
            #Now we have a MAC address, so we need a serial number.
            #Note that we should test for length using a regex like
            #above, but for now we'll leave it out for testing.
            if(defined($command[2])) {
               print_console("Starting test on position $focus : ");
               print_console("Mac -> $command[1], SN -> $command[2]", 1);
               #box_text("Testing", "$command[0]", $focus-1);
               my @args;
               if ($command[0] == 2009) {  # add bit to reset board if requested
                 $selected_op |= 0x10;               
               }
               push(@args, $selected_op);
               push(@args, $focus);
               push(@args, $command[1]);
               push(@args, $command[2]);
               push(@args, $config{product});
               push(@args, $config{initials});
               #Now go and do this in another process.
               print_console("Forking $programmer");
               fork_cmd($programmer, @args);
               #Clear these for the next time around
               @args = ();
               @command = ();
            }
            else {
               print_console("Enter or scan a serial to start test.");
            }
         }
         else {
            print_console("Enter or scan a MAC Address");
         }
      }
      elsif($command[0] == 2001) {
         print_console("Canceling test on $focus");
         box_text("Canceling...", "", $focus -1);
         #Clear the other parameters from @command
         @command = ();
         #Get the child PID
         open(PID, $testrootdir . "pos". ($focus) . "/pid");
         my $pid = <PID>;
         close(PID);
         #Kill it
         if(defined($pid)) {
            kill(15, $pid);
         }
         #Get rid of the PID and test status
         truncate($testrootdir . "pos". $focus . "/pid", 0);
         truncate($testrootdir . "pos". $focus . "/status.txt", 0);
         #Update the box
         box_text("", "", $focus-1);
      }
      elsif($command[0] == 2002) {
         print_console("Printing label for $focus");
         @command = ();
         #Put command to reprint here
         my $source = $testrootdir . "pos" . $focus . "/label.tmp";
         my $destination = $testrootdir . "print/" . $focus . "_" . time;
         `cp $source $destination`;
      }
      elsif($command[0] == 2003) {
         print_console("Sending logs to Getinge");
         @command = ();
         #Go into each test position directory and upload the logs
         my $upload = Archive::Zip->new();
         $upload->addTree($config{testrootdir}, 'netcom');
         my $zipfile = "/tmp/" . "complete_netcom_log." . time() . ".zip";
         if($upload->writeToFileNamed($zipfile) != AZ_OK) {
            print_console("Error writing zip file");
         }
         else {
            #Upload
            #Note that we need to come up with a proper URL for this part $log
            #Was 
            #my $output = `curl -sF file=\@$zipfile "http://tss.t-doc.com/upload/progress_upload.asp" | grep "successfully uploaded" > /dev/null`;
            my $output = `curl -sF file=\@$zipfile "http://tss.t-doc.com/upload/progress_upload.asp" | grep "successfully uploaded" > /dev/null`;
            #print STDERR $?;
            if($? != 0) {
               print_console("Error sending file to Getinge.");
            }
            else {
               print_console("Sent $zipfile to Getinge.");
            }
         }  
      }
      elsif($command[0] == 2004) {
         print_console("Install/Replace cable");
         @command = ();
      }
      elsif($command[0] == 2006) {
         print_console("Rebooting");
         system("sudo /sbin/reboot");
      }
      elsif($command[0] == 2007) {
         print_console("Shutting down");
         system("sudo /sbin/shutdown -h now");
      }
      elsif($command[0] == 2008) {
         #Just to test some other things...
         info_box("Test", "This is a test.");
         @command = ();
      }
   }
}

#Update the output window in the shell.  This is a hack
#to shift text up like a console.  There may be a better
#way to do this.
sub print_console {
   my $string = $_[0];
   my $no_newline = $_[1];
   
   open(LOG, ">>$term_log");
   
   my @box_array = split(/\n/, $output_box->text());
   if($no_newline) {             #Just concatenate to the last line
      $box_array[-1] .= $string;
      print LOG "\b" . $string;  #Backspace to get the newline in the log, before appending
   }
   else {
      push(@box_array, $string);
      print LOG localtime() . " " . $string . "\n";
   }
   close(LOG);
   if(@box_array > 6) {
      shift(@box_array);
   }
   $string = join("\n", @box_array);
   $output_box->text($string);
   $output_box->draw();
}

#Set the 'focus' (not Curses focus, but which test position we want)
#by changing the border color of a particular position box.  We want
#to make just one position green and the rest red, as that's the
#point of focus :).
sub set_focus {
   state $focus = 0;
   my $pos = $_[0];
   
   if(defined($pos)) {
      $focus = $pos;
      for(my $i = 0; $i < $boards; $i++) {
         if($i == $pos) {
            $board_windows[$i]{-bfg} = 'green';
            $board_windows[$i]->draw();
         }
         else {
            $board_windows[$i]{-bfg} = 'red';
            $board_windows[$i]->draw();
         }
      }
   }
   return $focus;
}

#Function for allowing better management of text in test position
#status boxes.  Take the status, test/programmer feedback, and
#box number.  If one of the first two is not present, we just update
#the other.
sub box_text {
   my ($status, $feedback, $pos) = @_;
   chomp($status);
   chomp($feedback);
   my $string;
   
   #Get what's already in the box
   my @box_array = ();
   @box_array = split(/\n\nProgrammer: /, $board_text[$pos]->text());
   
   if(!defined($box_array[0])) {
      $box_array[0] = "";
   }
   if(!defined($box_array[1])) {
      $box_array[1] = "";
   }
   
   if(defined($status)) {
      $box_array[0] = $status;
   }
   
   if(defined($feedback)) {
      $box_array[1] = $feedback;
   }
   
   $string = join("\n\nProgrammer: ", @box_array);

   $board_text[$pos]->text($string);
   $board_text[$pos]->draw();
}

#This function will let us update the info box, which is on the side in
#blue.  Takes two parameters: a label, and a value.  These will appear in
#this box like this:
#  Label: Value
#To add or clear a label, use the function like this:
#  info_box("Label", "");
#That is, a label name (no colon), and an empty value.   After that, one
#can change the value of a label, or create a new one and initialize
#the contents like this:
#  info_box("Label", "New value.");
#A label can be deleted by setting the text to <delete>, like this:
#  info_box("Label", "<delete>");
#The "Sam-ba Status" label is special, as it appears at a the bottom of 
#the list of things in there.  All you need to do to make it come up is
#assign a value; it can't be deleted.
sub info_box {
   my $label = $_[0];
   my $value = $_[1];
   
   if(!defined($label)) {
      $label = '';
   }
   if(!defined($value)) {
      $value = '';
   }
   
   my $text = $status_box->text();
   my @lines = split(/\n/, $text);
   
   my $flag = 1;
   my $samba;
   
   #We go through this to avoid using
   #wacky and complex data structures.
   
   my @outarray;
   my $element;
   
   foreach(@lines) {
      my @tmp = split(/:  /, $_);
      if($tmp[0] eq $label) {
         #We found it, so we don't have to add it later.
         $flag = 0;
         
         if($value ne "<delete>") {
            #If we have the Sam-ba Status label, we want
            #to save this for the end.
            if($label eq "Sam-ba Status") {
               $element = $tmp[0] . ":  " . $value;
            }
            else {
               #Update the line
               push(@outarray, $tmp[0] . ":  " . $value);
            }
         }
      }
      #Otherwise, just copy to the output array
      else {
         #Check for Sam-ba Status again...
         if($tmp[0] eq "Sam-ba Status") {
            if(!defined($tmp[1])) {
               $tmp[1] = '';
            }
            $element = $tmp[0] . ":  " . $tmp[1];
         }
         else {
            push(@outarray, $tmp[0] . ":  " . $tmp[1]);
         }
      }
   }
   
   #In this case, we looped through but didn't find the 
   #label.
   if($flag) {
      push(@outarray, $label . ":  " . $value);
   }
   
   #If it's there, push the Sam-ba Status line onto the array.
   if($element) {
      push(@outarray, $element);
   }
   
   #Join @outarray together into one string
   my $string = join("\n", @outarray);
   
   #Put it in the status box
   $status_box->text($string);
   
   #Redraw
   $status_box->draw();
}


#This function forks a new process and executes the command we want.
#This way the command can run and leave our main script alone.  Note
#that it is the responsibility of the called program to write its PID
#file, and delete it before it dies.  As arguments, take the path to
#the script (shifting it from the input array), and everything
#else forms the array for the arguments.
sub fork_cmd {
   my $command = shift;
   my @args = @_;
   my $pid = fork();
   
   #Now we're forked.  If the PID is 0, then
   #we're the child process, so we do something
   #and exit.  Otherwise we're the parent, so
   #we just go back to what we were doing.
   if(defined($pid) && $pid == 0) {
      #Child process
      #Make it shut up
      close(STDOUT);
      close(STDERR);
      
      #Note that although we closed those handles above,
      #we can override this in whatever we execute next
      #by opening them up again and redirecting them
      #to files.
      
      #Do our other script
      exec($command, @args);
      exit 0;
   }
   
   return $pid;
}

#This function kills things.  If something needs killing, put the
#command to do so in here.  We call this before exiting.
sub kill_stuff {
   my $res;
   $res = `/usr/bin/sudo /usr/bin/killall start_sam-ba.sh`;
   $res = `/usr/bin/sudo /usr/bin/killall sam-ba`;
   $res = `/usr/bin/sudo /usr/bin/killall printqueue.pl`;
   
   return 0;
}
