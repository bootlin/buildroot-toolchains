#!/bin/sh
TARGETDIR=$1
BOARDDIR=board/getinge/netcom/

# Allow login as root on ttyS4
if ! grep -q ttyS4 $TARGETDIR/etc/securetty ; then
	echo "ttyS4" >> $TARGETDIR/etc/securetty ;
fi

# Add some generic lines to the persistent-files.conf file
grep -q "/etc/conf.d/net.eth*" $TARGETDIR/etc/persistent-files.conf || \
    echo "/etc/conf.d/net.eth*" >> $TARGETDIR/etc/persistent-files.conf
grep -q "/etc/conf.d/hostname" $TARGETDIR/etc/persistent-files.conf || \
    echo "/etc/conf.d/hostname" >> $TARGETDIR/etc/persistent-files.conf

# Copy the rootfs additions
cp -a $BOARDDIR/rootfs-additions/* $TARGETDIR/
