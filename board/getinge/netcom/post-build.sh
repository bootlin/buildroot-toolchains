#!/bin/sh
TARGETDIR=$1
BOARDDIR=board/getinge/netcom/

# Allow login as root on ttyS4
if ! grep -q ttyS4 TARGETDIR/etc/securetty ; then
	echo "ttyS4" >> $TARGETDIR/etc/securetty ;
fi

# Configuration directory
mkdir -p $TARGETDIR/etc/conf.d/

# Init scripts
install -D -m 0755 $BOARDDIR/S02gpio $TARGETDIR/etc/init.d/S02gpio
install -D -m 0755 $BOARDDIR/S02mtd $TARGETDIR/etc/init.d/S02mtd
install -D -m 0755 $BOARDDIR/S03extract-mac $TARGETDIR/etc/init.d/S03extract-mac
install -D -m 0755 $BOARDDIR/S04extract-bootblock $TARGETDIR/etc/init.d/S04extract-bootblock

# Zcip configuration script
install -D -m 0644 $BOARDDIR/zcip.script $TARGETDIR/etc/zcip.script

# U-Boot fw_printenv/fw_setenv configuration file
install -D -m 0644 $BOARDDIR/fw_env.config $TARGETDIR/etc/fw_env.config