TOOLCHAIN=gcc-arm-none-eabi-8-2018-q4-major
export INSTALL_ROOT=`pwd`
TOOLS_PATH=$INSTALL_ROOT/tools
CROSS_PATH=$TOOLS_PATH/$TOOLCHAIN/bin
export PATH=$TOOLS_PATH/bin:$CROSS_PATH:$PATH

# Path to cross-tools
export CROSS_COMPILE=arm-none-eabi-
# ARCH for U-Boot build
export ARCH=arm
