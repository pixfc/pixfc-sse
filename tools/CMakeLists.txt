
# TODO: update tools so they only use the public interface rather than internal headers
include_directories(AFTER include ../src)

if(NOT WIN32)
	add_definitions(-Wno-trigraphs)
	if(PIXFC_TARGET_PROCESSOR MATCHES "intel")
		add_definitions(-msse2 -mssse3)
	endif(PIXFC_TARGET_PROCESSOR MATCHES "intel")
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

