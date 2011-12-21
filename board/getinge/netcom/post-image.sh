#!/bin/sh
BINARIES_DIR=$1

${BINARIES_DIR}/../host/usr/bin/fwupgrade-tool \
    -o ${BINARIES_DIR}/fw.img                  \
    -p kernel:${BINARIES_DIR}/uImage           \
    -p rootfs:${BINARIES_DIR}/rootfs.jffs2     \
    -i 0x2424

