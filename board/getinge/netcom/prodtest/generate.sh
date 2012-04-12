#!/bin/sh
BINARIES_DIR=$1
BUILD_DIR=$2

# Version information
MAJOR_VER=0
MINOR_VER=06

# find sam-ba location
IFS=:
for p in $PATH
do
  if echo "$p" | grep sam-ba &> /dev/null ; then
    samba_path=$p
    break;
  fi
done
# reset IFS if necessary 
if [ -f "$samba_path/sam-ba" ]; then
  echo "Using sam-ba found at: $samba_path"
else
  echo "Could not find sam-ba executable"
  exit 1
fi

# Remove build dir
rm -Rf ${BUILD_DIR}

# Make directories
mkdir -p ${BUILD_DIR}/images
mkdir -p ${BUILD_DIR}/sam-ba

# Copy image files to be flashed
cp ${BINARIES_DIR}/dataflash_at91sam9m10g45ek.bin ${BUILD_DIR}/images
cp ${BINARIES_DIR}/u-boot-env.bin                 ${BUILD_DIR}/images
cp ${BINARIES_DIR}/u-boot.bin                     ${BUILD_DIR}/images
cp ${BINARIES_DIR}/uboot.script.img               ${BUILD_DIR}/images

# Copy script files
pwd
cp -R board/getinge/netcom/prodtest/files/* ${BUILD_DIR}

# Copy sam-ba
cp -R ${samba_path}/* ${BUILD_DIR}/sam-ba

# Clean up sam-ba a bit by deleting some unneeded files
ls -1d ${BUILD_DIR}/sam-ba/applets/at91lib/boards/* | grep -v at91sam9g45-ek | xargs rm -Rf
ls -1d ${BUILD_DIR}/sam-ba/tcl_lib/* | grep -v at91sam9g45-ek | grep -v common | grep -v devices | grep -v boards.tcl | xargs rm -Rf

# Create version file
echo "${MAJOR_VER}.${MINOR_VER}" > ${BINARIES_DIR}/netcomtest.vers

# Create archive
tar -czf ${BINARIES_DIR}/netcomtest-${MAJOR_VER}.${MINOR_VER}.tar.gz -C ${BUILD_DIR} .


