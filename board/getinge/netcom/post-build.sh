#!/bin/sh
TARGETDIR=$1
BOARDDIR=board/getinge/netcom/

# Allow login as root on ttyS4
if ! grep -q ttyS4 $TARGETDIR/etc/securetty ; then
	echo "ttyS4" >> $TARGETDIR/etc/securetty ;
fi

# Copy the rootfs additions
cp -a $BOARDDIR/rootfs-additions/* $TARGETDIR/
