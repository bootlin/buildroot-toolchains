#!/bin/sh
BINARIES_DIR=$1

SWNUM=$(. board/getinge/netcom/rootfs-additions/usr/share/release/variables ; \
    echo $SWNUM)

# The build ID file has been created or updated by the post-build
# script executed previously
BUILD_ID=$(cat .netcom_build_id)

HWID=0x2424

${BINARIES_DIR}/../host/usr/bin/fwupgrade-tool  \
    -o ${BINARIES_DIR}/${SWNUM}_${BUILD_ID}.bin \
    -p kernel:${BINARIES_DIR}/uImage            \
    -p rootfs:${BINARIES_DIR}/rootfs.jffs2      \
    -i ${HWID}

# Generate production files
sh ./board/getinge/netcom/prodtest/generate.sh ${BINARIES_DIR} output/images/prodtest
