#!/bin/sh
BINARIES_DIR=$1

SWNUM=$(. board/getinge/netcom/rootfs-additions/usr/share/release/variables ; \
    echo $SWNUM)

HWID=0x2424

${BINARIES_DIR}/../host/usr/bin/fwupgrade-tool \
    -o ${BINARIES_DIR}/${SWNUM}.bin           \
    -p kernel:${BINARIES_DIR}/uImage           \
    -p rootfs:${BINARIES_DIR}/rootfs.jffs2     \
    -i ${HWID}

# Generate production files
sh ./board/getinge/netcom/prodtest/generate.sh ${BINARIES_DIR} output/images/prodtest
