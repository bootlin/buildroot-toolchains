#!/bin/bash

TARGET_DIR=$1

BRANCH=$(git symbolic-ref -q HEAD)
BRANCH=${BRANCH##refs/heads/chip/}

BUILDROOT_GITHASH="$(cat buildroot_githash)"
BUILDROOT_GITHASH="${BUILDROOT_GITHASH:0:8}"

BUILD="$(cat build)"

cat <<EOF >${TARGET_DIR}/etc/issue
Welcome to CHIP Buildroot-${BRANCH} build ${BUILD} rev ${BUILDROOT_GITHASH}

CHIP Buildroot contains various open source software.

The source code can be downloaded from:
http://opensource.nextthing.co/chip/buildroot/${BRANCH}/${BUILD}/build${BUILD}.tar.gz

EOF
