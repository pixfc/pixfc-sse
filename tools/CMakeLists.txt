
# Source files for test apps
set(TEST_UTIL_SRC_LIST
	test-utils.c
)

# Header files for test apps
set(TEST_UTIL_HDR_LIST
	test-utils.h
	rgb_image.h
)

# TODO: update tools so they only use the public interface rather than internal headers
include_directories(AFTER include ../src)

if(NOT WIN32)
	add_definitions(-Wno-trigraphs)
endif(NOT WIN32)

#
# UNIT TESTING TARGET
#
# Create groups for XCode and VS
source_group("source files" FILES unit-testing.c ${TEST_UTIL_SRC_LIST})
source_group("header files" FILES ${TEST_UTIL_HDR_LIST})
# Create unit testing app
add_executable(unit-testing unit-testing.c ${TEST_UTIL_SRC_LIST} ${TEST_UTIL_HDR_LIST})
if(WIN32 OR APPLE)
	target_link_libraries(unit-testing pixfc-sse)
else(WIN32 OR APPLE)
	target_link_libraries(unit-testing pixfc-sse rt)
endif(WIN32 OR APPLE)


#
# FILE CONVERT TARGET
#
# Create groups for XCode and VS
source_group("source files" FILES convert-to-file.c ${TEST_UTIL_SRC_LIST})
source_group("header files" FILES ${TEST_UTIL_HDR_LIST})
# Create unit testing app
add_executable(convert-to-file convert-to-file.c ${TEST_UTIL_SRC_LIST} ${TEST_UTIL_HDR_LIST})
if(WIN32 OR APPLE)
	target_link_libraries(convert-to-file pixfc-sse)
else(WIN32 OR APPLE)
	target_link_libraries(convert-to-file pixfc-sse rt)
endif(WIN32 OR APPLE)

#
#  VISUAL CHECK TARGET
#
# Create groups for XCode and VS
source_group("source files" FILES visual-check.c ${TEST_UTIL_SRC_LIST})
source_group("header files" FILES ${TEST_UTIL_HDR_LIST})
# Create visual check app
add_executable(visual-check visual-check.c ${TEST_UTIL_SRC_LIST} ${TEST_UTIL_HDR_LIST})
if(WIN32 OR APPLE)
	target_link_libraries(visual-check pixfc-sse)
else(WIN32 OR APPLE)
	target_link_libraries(visual-check pixfc-sse rt)
endif(WIN32 OR APPLE)

