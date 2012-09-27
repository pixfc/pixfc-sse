SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR armv6l)

# -mcpu
SET(PIXFC_ARM_TARGET_PROCESSOR arm1176jzf-s)

# -mfloat_abi
SET(PIXFC_ARM_TARGET_FLOAT_ABI hard)

# -mfpu
SET(PIXFC_ARM_TARGET_FPU vfp)

# compiler
# Uncomment/udpate when cross compiling
# The current value points to the version of GCC built specifically
# for cross-compiling for RPi. It can be installed with :
# sudo git clone https://github.com/raspberrypi/tools.git /opt/rpi-xtools
#SET(CMAKE_C_COMPILER /opt/rpi-xtools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-gcc)
