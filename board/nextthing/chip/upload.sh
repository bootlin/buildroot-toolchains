#!/bin/bash

BRANCH=$(git symbolic-ref -q HEAD)
BRANCH=${BRANCH##refs/heads/chip/}
BRANCH=${BRANCH##refs/heads/nextthing/*/}

BUILD_NUMBER=$1
S3_DEST=s3://opensource.nextthing.co/chip/buildroot/${BRANCH}
# IMPORTANT: in order to have the md5sum as an etag one has to use --disable-multipart
# also see: http://docs.aws.amazon.com/AmazonS3/latest/API/RESTCommonResponseHeaders.html
S3_OPTIONS="--acl-public --no-guess-mime-type --disable-multipart"

if s3cmd put ${S3_OPTIONS} --recursive output/images ${S3_DEST}/${BUILD_NUMBER}/ |tee s3cmd_out.txt
then
  echo "${BUILD_NUMBER}" >upload_buildno
  
  s3cmd put ${S3_OPTIONS} upload_buildno ${S3_DEST}/${BUILD_NUMBER}/build 

  cat s3cmd_out.txt |grep Public |head -n1 |sed -e 's/.*\(http:\/\/.*\/[0-9]\+\)\/.*/\1/;' >latest
	s3cmd put ${S3_OPTIONS} latest ${S3_DEST}/latest

  s3cmd put ${S3_OPTIONS}	output/build/uboot*/spl/sunxi-spl-with-ecc.bin ${S3_DEST}/${BUILD_NUMBER}/images/sunxi-spl-with-ecc.bin
	#cat s3cmd_out.txt |grep Public |head -n1 |sed -e 's/.*http:/http:/; s/\(http:.*\/\).*\/.*/\1/' >os_latest
	#s3cmd put ${S3_OPTIONS} --add-header="x-amz-website-redirect-location:/chip/buildroot/${BRANCH}/${BUILD_NUMBER}/rootfs.ubi" os_latest s3://opensource.nextthing.co/chip/buildroot/stable/latest
fi
