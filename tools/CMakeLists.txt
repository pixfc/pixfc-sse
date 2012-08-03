
# TODO: update tools so they only use the public interface rather than internal headers
include_directories(AFTER include ../src)

list(APPEND TOOLS_LIBS pixfc-sse)

if(APPLE)
	add_definitions(-Wno-trigraphs)
elseif(UNIX)
	list(APPEND TOOLS_LIBS rt)
	add_definitions(-Wno-trigraphs)
endif(APPLE)

#
# UNIT TESTING TARGET
#
# Create groups for XCode and VS
source_group("source files" FILES unit-testing.c test-utils.c)
source_group("header files" FILES test-utils.h)
# Create unit testing app
add_executable(unit-testing unit-testing.c test-utils.c test-utils.h)
target_link_libraries(unit-testing ${TOOLS_LIBS})


#
# FILE CONVERT TARGET
#
# Create groups for XCode and VS
source_group("source files" FILES convert-to-file.c test-utils.c rgb_image.c)
source_group("header files" FILES test-utils.h rgb_image.h)
# Create unit testing app
add_executable(convert-to-file convert-to-file.c test-utils.c rgb_image.c test-utils.h rgb_image.h)
target_link_libraries(convert-to-file ${TOOLS_LIBS})

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

