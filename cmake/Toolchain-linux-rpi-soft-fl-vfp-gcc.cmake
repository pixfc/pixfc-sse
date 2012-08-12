SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR armv6l)

# -mcpu
SET(PIXFC_ARM_TARGET_PROCESSOR arm1176jzf-s)

# -march
# not set as it conflicts with -mcup above
#SET(PIXFC_ARM_TARGET_ARCH armv6)

# -mfloat_abi
SET(PIXFC_ARM_TARGET_FLOAT_ABI softfp)

# -mfp
SET(PIXFC_ARM_TARGET_FPU vfp)

# compiler
SET(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabi-gcc)
