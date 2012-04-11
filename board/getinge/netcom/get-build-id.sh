#!/bin/bash

# This script returns on its standard output the current build id.
#
# The logic is as follows:
#
#  The build id increments monotonically, starting from an arbitrary
#  value, stored in $STARTING_BUILD_ID. This incrementation takes
#  place at every invocation of 'make' in Buildroot.
#
#  When the version changes in the
#  board/getinge/netcom/rootfs-additions/usr/share/release/variables
#  file, the build id is reset to $STARTING_BUILD_ID, and we restart
#  incrementing from there.
#
# To implement this logic, we keep two hidden files, $BUILD_ID_FILE
# and $VERSION_ID_FILE in the top directory of the Buildroot tree, to
# store the build id and version string of the last build.

# The starting build id, used as the build id when the version string
# has changed.
STARTING_BUILD_ID=1000

# Hidden file where the build id of the previous build is stored and
# kept accross make invocations.
BUILD_ID_FILE=.netcom_build_id

# Hidden file where the version string of the previous build is stored
# and kept accross make invocations.
VERSION_ID_FILE=.netcom_version_id

# Compute the current version string, by reading the 'variables' file.
VERSION_STRING=$(. board/getinge/netcom/rootfs-additions/usr/share/release/variables ; \
    printf "%s.%s.%s" $VERSION_MAJOR $VERSION_MINOR $VERSION_REVISION)

# First, determine if the version has changed.
VERSION_CHANGED=0
if [ ! -f ${VERSION_ID_FILE} ] ; then
    # It's the case if the hidden file containing the version does not
    # exist...
    VERSION_CHANGED=1
else
    PREVIOUS_VERSION_STRING=$(cat ${VERSION_ID_FILE})
    if [ ${VERSION_STRING} != ${PREVIOUS_VERSION_STRING} ] ; then
	# ...or if it contains a version string different to the
	# current one.
	VERSION_CHANGED=1
    fi
fi

# Store the current version string
echo ${VERSION_STRING} > ${VERSION_ID_FILE}

# If the hidden file containing the previous build id does not exist,
# or if the version string has changed, reset the build id to
# $STARTING_BUILD_ID. Otherwise, simply increment it.
if [ ! -f ${BUILD_ID_FILE} -o ${VERSION_CHANGED} -eq 1 ] ; then
    BUILD_ID=${STARTING_BUILD_ID}
else
    BUILD_ID=$(cat ${BUILD_ID_FILE})
    BUILD_ID=$((BUILD_ID+1))
fi

# Store the new build id in the hidden file
echo ${BUILD_ID} > ${BUILD_ID_FILE}

# Display it on the standard output
echo ${BUILD_ID}
