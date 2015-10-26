#!/bin/bash

BRANCH=$(git symbolic-ref -q HEAD)
BRANCH=${BRANCH##refs/heads/chip/}
BRANCH=${BRANCH##refs/heads/nextthing/*/}

BUILD_NUMBER=$1
S3_DEST=s3://opensource.nextthing.co/chip/buildroot/${BRANCH}

tar czf /tmp/CHIP-buildroot-build${BUILD_NUMBER}.tar.gz ../build || exit $?

s3cmd put --acl-public --no-guess-mime-type /tmp/CHIP-buildroot-build${BUILD_NUMBER}.tar.gz ${S3_DEST}/${BUILD_NUMBER}/build${BUILD_NUMBER}.tar.gz || exit $?

rm /tmp/CHIP-buildroot-build${BUILD_NUMBER}.tar.gz || exit $?
