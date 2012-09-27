SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR armv6l)

# -mcpu
SET(PIXFC_ARM_TARGET_PROCESSOR arm1176jzf-s)

# -march
# not set as it conflicts with -mcpu above
#SET(PIXFC_ARM_TARGET_ARCH armv6)

# -mfloat_abi
SET(PIXFC_ARM_TARGET_FLOAT_ABI softfp)

# -mfpu
SET(PIXFC_ARM_TARGET_FPU vfp)

# compiler 
# Uncomment/update when cross compiling.
# The default value points to GCC in the Linaro toolchain
# as installed on Ubuntu with apt-get install gcc-arm-linux-gnueabi.
# See https://launchpad.net/~linaro-maintainers/+archive/toolchain
#SET(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabi-gcc)
