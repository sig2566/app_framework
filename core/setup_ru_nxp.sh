directory=$SW_PACKAGER_BIN_PATH
if [ -d "$directory" ]; then
echo "Directory exists"
else
echo "Directory does not exist, error"
exit 1
fi

export PLATFORM_NAME=ru_nxp
PWD=`pwd`
export BIN=$PWD/bin
export SW_PACKAGER_SYS_ROOT_PATH=$directory/SW_PACKAGER_SYS_ROOT_PATH
export DPDK_INC=$directory/DPDK_INC
export DPDK_LIB=$directory/DPDK_LIB
export CROSS_COMPILE=/opt/gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-

