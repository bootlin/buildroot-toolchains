#!/bin/sh
TARGETDIR=$1
BOARDDIR=board/getinge/netcom/

# Allow login as root on ttyS4
if ! grep -q ttyS4 $TARGETDIR/etc/securetty ; then
	echo "ttyS4" >> $TARGETDIR/etc/securetty ;
fi

# Copy the rootfs additions
cp -a $BOARDDIR/rootfs-additions/* $TARGETDIR/

# Generate the factory default image
mkfs.jffs2 -p -e 128 -o $TARGETDIR/usr/share/conf.d/defaults.img -d $BOARDDIR/conf.d/

# Copy the configuration version
cp $BOARDDIR/conf.d/version $TARGETDIR/usr/share/conf.d/