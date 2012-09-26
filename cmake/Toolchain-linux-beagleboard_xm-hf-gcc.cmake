SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR armv7-a)

# -mcpu
SET(PIXFC_ARM_TARGET_PROCESSOR cortex-a8)

# -march
SET(PIXFC_ARM_TARGET_ARCH armv7-a)

# -mfloat_abi
SET(PIXFC_ARM_TARGET_FLOAT_ABI hard)

# -mfpu
SET(PIXFC_ARM_TARGET_FPU vfpv3-d16)

# compiler (uncomment/update when cross compiling)
#SET(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc)
