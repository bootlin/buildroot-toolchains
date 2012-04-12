#!/bin/bash
# The following creates the links to program via sam-ba and then waits for a device to be connected
# it will skip the scanner port.
# call this script with the device name (ie. /dev/ttyACM0) of the barcode scanner
scanner_port=$1
nextPort=10
mkdir -p /var/netcom/samba-output
echo "Starting" > /var/netcom/samba-status.txt
for i in {0..8}
do
  if [ "$scanner_port" != "/dev/ttyACM$i" ]; then
    ln -s /dev/ttyACM$i /dev/ttyUSB$nextPort &> /dev/null
    nextPort=$((nextPort+1))
  fi
done
echo "Idle" > /var/netcom/samba-status.txt
while [ true ]; do
  for i in {10..17}
  do
    if [ -e /dev/ttyUSB$i ]; then
      echo -n "Programming device on /dev/ttyUSB$i" > /var/netcom/samba-status.txt
      ./sam-ba/sam-ba /dev/ttyUSB$i at91sam9g45-ekes ./samba-scripts/samba-script.tcl &> /var/netcom/samba-output/$i.log && echo ": Done" >> /var/netcom/samba-status.txt || echo ": Failed" >> /var/netcom/samba-status.txt
    fi
  done
  sleep 1;
done

