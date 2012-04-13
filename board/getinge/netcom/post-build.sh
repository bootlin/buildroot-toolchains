#!/bin/sh
TARGETDIR=$1
BOARDDIR=board/getinge/netcom/

# Allow login as root on ttyS4
if ! grep -q ttyS4 $TARGETDIR/etc/securetty ; then
	echo "ttyS4" >> $TARGETDIR/etc/securetty ;
fi

# Disable login with the 'default' account
sed -i 's/^default::/default:*:/' $TARGETDIR/etc/shadow

sed -i '/.*remount,rw.*/d' $TARGETDIR/etc/inittab

# Set up the default root password. Generated with the 'mkpasswd'
# utility.
sed -i 's/^root::/root:GlUwvv8.UEkdE:/' $TARGETDIR/etc/shadow

# Add some generic lines to the persistent-files.conf file
grep -q "/etc/conf.d/net.eth*" $TARGETDIR/etc/persistent-files.conf || \
    echo "/etc/conf.d/net.eth*" >> $TARGETDIR/etc/persistent-files.conf
grep -q "/etc/conf.d/hostname" $TARGETDIR/etc/persistent-files.conf || \
    echo "/etc/conf.d/hostname" >> $TARGETDIR/etc/persistent-files.conf

# Copy the rootfs additions
cp -a $BOARDDIR/rootfs-additions/* $TARGETDIR/

# Fixup the build time
BUILD_TIME=$(LANG=C date '+%b %d %Y %H:%M')
sed -i "s/__BUILD_TIME__/${BUILD_TIME}/" $TARGETDIR/usr/share/release/variables

# Find the build ID
BUILD_ID=$($BOARDDIR/get-build-id.sh)
sed -i "s/__BUILD__/${BUILD_ID}/" $TARGETDIR/usr/share/release/variables
