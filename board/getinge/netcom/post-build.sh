#!/bin/sh
TARGETDIR=$1
BOARDDIR=board/getinge/netcom/
IMAGEDIR=${TARGETDIR}/../images/

# Allow login as root on ttyS4
if ! grep -q ttyS4 $TARGETDIR/etc/securetty ; then
	echo "ttyS4" >> $TARGETDIR/etc/securetty ;
fi

# Allow login as root on the first four pts/x so that telnet logins
# work
for i in $(seq 0 3) ; do
    if ! grep -q "pts/$i" $TARGETDIR/etc/securetty ; then
	echo "pts/$i" >> $TARGETDIR/etc/securetty ;
    fi
done

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

# Remove the default link, we need to point to the localtime file in
# /etc/conf.d/.
rm ${TARGETDIR}/etc/localtime
ln -sf /etc/conf.d/localtime ${TARGETDIR}/etc/localtime

# Need an empty directory for vsftpd to be happy
mkdir -p ${TARGETDIR}/usr/share/empty/

# Add init scripts links for vsftpd and telnetd
ln -sf /etc/init.d/netcom-common ${TARGETDIR}/etc/init.d/S70vsftpd
ln -sf /etc/init.d/netcom-common ${TARGETDIR}/etc/init.d/S70telnetd

