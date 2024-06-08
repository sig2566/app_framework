#!/bin/bash
directory=$1
if [ -d "$directory" ]; then
echo "Directory exists"
else
echo "Directory does not exist, create"
mkdir -p $directory
fi
cp -fr $SW_PACKAGER_SYS_ROOT_PATH $directory/SW_PACKAGER_SYS_ROOT_PATH
cp -fr $DPDK_INC $directory/DPDK_INC
cp -fr $DPDK_LIB $directory/DPDK_LIB
