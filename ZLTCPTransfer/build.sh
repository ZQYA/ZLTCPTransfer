#!/bin/bash

FRM_name="TCPTransfer"

MMTP_path="../MMTP"
MMTP_branch="develop"

TCPTransfer_path=`pwd`
TCPTransfer_branch="develop"

#update MMTP
cd ${MMTP_path}
git checkout .
git clean -df
git fetch
git checkout ${MMTP_branch}
git rebase origin/${MMTP_branch}

#update ZLTCPTransfer
cd ${TCPTransfer_path}
git checkout .
git clean -df
git fetch
git checkout ${TCPTransfer_branch}
git rebase origin/${TCPTransfer_branch}

#build
#xcodebuild -configuration "Release" -target "${FRM_name}"  clean build
