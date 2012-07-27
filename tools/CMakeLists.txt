
# TODO: update tools so they only use the public interface rather than internal headers
include_directories(AFTER include ../src)

if(NOT WIN32)
	add_definitions(-Wno-trigraphs)

        if(PIXFC_TARGET_ARCH MATCHES "Intel")
                add_definitions(-msse2 -mssse3)
                if(PIXFC_TARGET_ARCH MATCHES "x86$")
                        add_definitions(-m32)
                elseif(PIXFC_TARGET_ARCH MATCHES "x86_64$")
                        add_definitions(-m64)
                else(PIXFC_TARGET_ARCH MATCHES "x86$")
                        message(FATAL_ERROR "Unknown Intel CPU " ${PIXFC_TARGET_ARCH})
                endif(PIXFC_TARGET_ARCH MATCHES "x86$")
        elseif(PIXFC_TARGET_ARCH STREQUAL "ARM")
                if(NOT DEFINED PIXFC_ARM_TARGET_PROCESSOR)
                        message(FATAL_ERROR "PIXFC_ARM_TARGET_PROCESSOR not defined - define in toolchain CMAKE file: " ${CMAKE_TOOLCHAIN_FILE})
                endif(NOT DEFINED PIXFC_ARM_TARGET_PROCESSOR)

                add_definitions(-mtune=${PIXFC_ARM_TARGET_PROCESSOR})
        endif(PIXFC_TARGET_ARCH MATCHES "Intel")
endif(NOT WIN32)

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
endif(DEBUG)

#
# UNIT TESTING TARGET
#
# Create groups for XCode and VS
source_group("source files" FILES unit-testing.c test-utils.c)
source_group("header files" FILES test-utils.h)
# Create unit testing app
add_executable(unit-testing unit-testing.c test-utils.c test-utils.h)
if(WIN32 OR APPLE)
	target_link_libraries(unit-testing pixfc-sse)
else(WIN32 OR APPLE)
	target_link_libraries(unit-testing pixfc-sse rt)
endif(WIN32 OR APPLE)


#
# FILE CONVERT TARGET
#
# Create groups for XCode and VS
source_group("source files" FILES convert-to-file.c test-utils.c rgb_image.c)
source_group("header files" FILES test-utils.h rgb_image.h)
# Create unit testing app
add_executable(convert-to-file convert-to-file.c test-utils.c rgb_image.c test-utils.h rgb_image.h)
if(WIN32 OR APPLE)
	target_link_libraries(convert-to-file pixfc-sse)
else(WIN32 OR APPLE)
	target_link_libraries(convert-to-file pixfc-sse rt)
endif(WIN32 OR APPLE)

#
#  VISUAL CHECK TARGET
#
# Create groups for XCode and VS
source_group("source files" FILES visual-check.c test-utils.c rgb_image.c)
source_group("header files" FILES test-utils.h rgb_image.h)
# Create visual check app
add_executable(visual-check visual-check.c test-utils.c rgb_image.c test-utils.h rgb_image.h)
if(WIN32 OR APPLE)
	target_link_libraries(visual-check pixfc-sse)
else(WIN32 OR APPLE)
	target_link_libraries(visual-check pixfc-sse rt)
endif(WIN32 OR APPLE)

#
#  VALIDATE OUTPUT TARGET
#
# Create groups for XCode and VS
source_group("source files" FILES validate-output.c test-utils.c rgb_image.c)
source_group("header files" FILES test-utils.h rgb_image.h)
# Create app
add_executable(validate-output validate-output.c test-utils.c rgb_image.c test-utils.h rgb_image.h)
if(WIN32 OR APPLE)
	target_link_libraries(validate-output pixfc-sse)
else(WIN32 OR APPLE)
	target_link_libraries(validate-output pixfc-sse rt)
endif(WIN32 OR APPLE)

