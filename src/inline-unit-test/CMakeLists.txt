cmake_minimum_required (VERSION 2.6)
project (PixFC_Inline_Unit_test C)

include(../../cmake/DetectTarget.cmake)

set(HEADER_DIR ..)
include_directories(AFTER .. ../../include/common)

if(NOT WIN32)
    if(PIXFC_TARGET_ARCH MATCHES "Intel")
        add_definitions(-msse2 -mssse3 -msse4.1)
        if(PIXFC_TARGET_ARCH MATCHES "x86$")
            add_definitions(-m32)
            message("Build setup for Intel 32-bit CPU")
        elseif(PIXFC_TARGET_ARCH MATCHES "x86_64$")
            add_definitions(-m64)
            message("Build setup for Intel 64-bit CPU")
        else(PIXFC_TARGET_ARCH MATCHES "x86$")
            message(FATAL_ERROR "Unknown Intel CPU " ${PIXFC_TARGET_ARCH})
        endif(PIXFC_TARGET_ARCH MATCHES "x86$")
    elseif(PIXFC_TARGET_ARCH STREQUAL "ARM")
        if(NOT DEFINED PIXFC_ARM_TARGET_PROCESSOR)
            message(FATAL_ERROR "PIXFC_ARM_TARGET_PROCESSOR not defined - define in toolchain CMAKE file: " ${CMAKE_TOOLCHAIN_FILE})
        endif(NOT DEFINED PIXFC_ARM_TARGET_PROCESSOR)

        message("Build setup for ${PIXFC_ARM_TARGET_PROCESSOR} CPU")
        add_definitions(-mtune=${PIXFC_ARM_TARGET_PROCESSOR})
    endif(PIXFC_TARGET_ARCH MATCHES "Intel")

endif(NOT WIN32)

if(FAKE_SSE4)
    add_definitions(-DFAKE_SSE41_BLENDV)
endif(FAKE_SSE4)

# Check if we are building in debug mode
if(DEBUG)
        include_directories(AFTER .)
        add_definitions(-DDEBUG)

        # Add compiler specific flags   
        if(APPLE)
                add_definitions(-ggdb -O0 -Wall -Winline -Wno-implicit-function-declaration )
        elseif(UNIX)
                add_definitions(-ggdb -O0 -Wall -Winline -Wno-trigraphs -Wno-implicit-function-declaration)
        elseif(WIN32)
#               add_definitions(-Wno-implicit-function-declaration)
        endif(APPLE)
else(DEBUG)
	if(NOT WIN32)
		add_definitions(-O3)
	endif(NOT WIN32)
endif(DEBUG)

# List of header files containing extern inlines definition
list(APPEND INLINE_FILENAME
    yuv_repack
    yuv_upsample
    yuv_to_rgb_convert
    rgb_downsample
    rgb_to_yuv_convert
	yuv_unpack
    rgb_pack
    rgb_unpack
    yuv_pack
)

# List of source files
list(APPEND SOURCE_LIST
	inline-unit-test.c
	inline-testing-common.c
	../common.c
)
# add inline source file to source file list
foreach(CUR_FILE ${INLINE_FILENAME})
        list(APPEND SOURCE_LIST "${CUR_FILE}.c")
endforeach(CUR_FILE)

# List of header files (build from INLINE_FILENAME list)
foreach(CUR_FILE ${INLINE_FILENAME})
        list(APPEND HEADER_LIST "../${CUR_FILE}.h")
endforeach(CUR_FILE)



source_group("source files" FILES ${SOURCE_LIST})
source_group("header files" FILES ${HEADER_LIST})
# Create unit testing app
add_executable(inline-unit-test ${SOURCE_LIST} ${HEADER_LIST})

