#!/bin/bash

BUILD_NUMBER=$1
BUILDROOT_HASH=$(git rev-parse HEAD)

echo ${BUILD_NUMBER}  >build
echo ${BUILDROOT_HASH} >buildroot_githash
echo "put version information into $PWD"
ls
