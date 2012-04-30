#!/bin/sh

# This script generates the NetCOM pass/fail labels
#
# zebra.sh labeltype dest_file [vars_file]
#  labeltype:  0=fail, 1=pass
#  vars can either be in the env or specified in a file

# This function starts a zebra label 
print_zebra_start()
{
  echo "^XA"
}

# This function ends a zebra label 
print_zebra_end()
{
  echo "^XZ"
}

# This function inserts graphics into zebra label
print_zebra_graphic()
{
cat resources/Getinge_logo_280x48.zpl
cat resources/book.zpl
cat resources/disposal.zpl
}

# This function reads the test log, removes those annoying lines full of "---"
#  and appends a "\&" to each line as required by the zebra for EOLs
print_zebra_test_log()
{
  echo "^FO10,190^FB800,9999,0,L"
  echo "^A0,12,14^FD"
  sed 's/--[-]*--//g' hwtestlog | sed 's/$/\\\&/g'
}

calc_test_position()
{
if [ "$_test_position" -le "4" ]; then
  _tp_x=$((447+$_test_position*25))
  _tp_y=129
else
  _tp_x=$((447+($_test_position-4)*25))
  _tp_y=154
fi
}

print_zebra_fail()
{
cat << EOF
^FWN^LH0,0^CI13
^PON
^FO10,$(($_yorg+255))^GB675,100,100,3^FS
^FO30,$(($_yorg+260))^A0,45,45^FR^FDDO NOT SHIP - Q.C. FAIL^FS
^FO30,$(($_yorg+300))^A0,22,22^FR^FD$_pcb_fail_text1^FS
^FO30,$(($_yorg+325))^A0,22,22^FR^FD$_pcb_fail_text2^FS
^FO700,$(($_yorg+290))^A0,22,22^FD$_pcb_rohs^FS
EOF
}

print_zebra_common()
{
((_yorg = $1))
calc_test_position
cat << EOF
^FWN^LH0,0^CI13
^PON
^FO30,$(($_yorg+27))^XGGetinge_logo_280x48,1,1^FS
^FO380,$(($_yorg+22))^BY2,2.5,^B3N,N,60,N,N^FV$_mac^FS
^FO505,$(($_yorg+90))^A0,34,34^FVMAC: $_mac^FS
^FO30,$(($_yorg+90))^A0,34,34^FD$_prod_name^FS
^FO30,$(($_yorg+135))^A0,36,36^FDP/N: $_pn^FS
^FO0,$(($_yorg+125))^GB850,50,1^FS
^FO355,$(($_yorg+125))^GB110,50,1^FS
^FO464,$(($_yorg+150))^GB101,25,1^FS
^FO490,$(($_yorg+125))^GB25,50,1^FS
^FO540,$(($_yorg+125))^GB25,50,1^FS
^FO$_tp_x,$(($_yorg+$_tp_y))^A0,22,22^FD$_test_position^FS
^FO365,$(($_yorg+155))^A0,22,22^FDLOC: $_test_location_code^FS
^FO585,$(($_yorg+132))^A0,22,22^FDDATE: $_test_date^FS
^FO585,$(($_yorg+155))^A0,22,22^FDOPER: $_operator^FS
^FO710,$(($_yorg+185))^BXN,2,200,36,36^FV${_pn},${_mac},$_swnum,$_swver,$_pcb_barcode^FS
^FO30,$(($_yorg+185))^A0,22,22^FDPCB PN:^FS
^FO200,$(($_yorg+185))^A0,22,22^FD$_pcb_item^FS
^FO30,$(($_yorg+209))^A0,22,22^FDPCB DATE:^FS
^FO200,$(($_yorg+209))^A0,22,22^FD$_pcb_date^FS
^FO465,$(($_yorg+185))^A0,22,22^FDPCB SER:^FS
^FO600,$(($_yorg+185))^A0,22,22^FD$_pcb_ser^FS
^FO465,$(($_yorg+209))^A0,22,22^FDPCB ORDER:^FS
^FO600,$(($_yorg+209))^A0,22,22^FD$_pcb_order^FS
^FO465,$(($_yorg+233))^A0,22,22^FDPCB FACTORY:^FS
^FO600,$(($_yorg+233))^A0,22,22^FD$_pcb_fact^FS
^FO700,$(($_yorg+290))^A0,22,22^FD$_pcb_rohs^FS
^FO760,$(($_yorg+290))^XGdisposal_280x48,1,1^FS
^FO30,$(($_yorg+360))^A0,18,18^FD$_battery_message^FS
^FO30,$(($_yorg+385))^A0,22,22^FDAssembled in $_assy_country^FS
^FO300,$(($_yorg+385))^GS,22,22^FDB^FS
^FO320,$(($_yorg+385))^A0,22,22^FD2012 GETINGE IT SOLUTIONS  GetingeNetCOM.com^FS
EOF
}

# This function prints a zebra label for a device that passed the Q.C. test
# it takes a single argument specifying the y offset of the printout
# which can be used to repeat the printout multiple times within a single label
print_zebra_pass()
{
((_yorg = $1))
cat << EOF
^FO365,$(($_yorg+132))^A0,22,22^FDQ.C. PASS^FS
^FO30,$(($_yorg+233))^A0,22,22^FDAUTO IP:^FS
^FO200,$(($_yorg+233))^A0,22,22^FD$_autoIP^FS
^FO30,$(($_yorg+257))^A0,22,22^FDSW NUM:^FS
^FO200,$(($_yorg+257))^A0,22,22^FD$_swnum^FS
^FO30,$(($_yorg+281))^A0,22,22^FDSW VER:^FS
^FO200,$(($_yorg+281))^A0,22,22^FD$_swver^FS
^FO30,$(($_yorg+305))^A0,22,22^FDSW RELEASE:^FS
^FO200,$(($_yorg+305))^A0,22,22^FD$_release^FS
^FO30,$(($_yorg+329))^A0,22,22^FDSW LICENSE:^FS
^FO200,$(($_yorg+329))^A0,22,22^FD$_license^FS
^FO700,$(($_yorg+310))^XGbook,1,1^FS
EOF
}

#Main code
  [ $# -lt 2 ] && echo "Not enough arguments" 1>&2 && exit 1;  # not enough args
  [ -f "$3" ] && . $3  # if a file of variables has been specified, invoke it
  [ -z "$_test_position" ] && echo "No test position specified" 1>&2 && exit 1;
  print_zebra_start > $2
  print_zebra_graphic >> $2
  print_zebra_common 0 >> $2
  case "$1" in
    0)
      print_zebra_fail 0 >> $2
      ;;
    1)
      print_zebra_pass 0 >> $2
      ;;
    *)
      echo "Unknown label type" 1>&2
      exit 1;
      ;;
  esac
  print_zebra_end >> $2
  exit 0;

