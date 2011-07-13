#!/bin/sh
TARGETDIR=$1
BOARDDIR=board/getinge/netcom/
echo "ttyS4" > $TARGETDIR/etc/securetty
install -D -m 0755 $BOARDDIR/S02gpio $TARGETDIR/etc/init.d/S02gpio
