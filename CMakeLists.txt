#
# PixFCSSE CMakeList file
#
# Copyright (C) 2011 PixFC Team (pixelfc@gmail.com)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public  License as published by the
# Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required (VERSION 2.6)
project (PixFC-SSE)


# PixFC source files
list(APPEND PIXFC_SRC_LIST
	pixfc-sse.c
	common.c
   	conversion_blocks.c
	conversion_routines_from_yuyv.c
	conversion_routines_from_yuyv_bt601.c
	conversion_routines_from_yuyv_bt709.c
	conversion_routines_from_uyvy.c
	conversion_routines_from_uyvy_bt601.c
	conversion_routines_from_uyvy_bt709.c
	conversion_routines_from_argb.c
	conversion_routines_from_argb_bt601.c
	conversion_routines_from_argb_bt709.c
)

# Source files for test apps
set(TEST_UTIL_SRC_LIST
	test-utils.c
	pixfmt_descriptions.c
)

# header files
file(GLOB PIXFC_HDR_LIST *.h)	



# List of header files containing extern inlines definitions that must
# be copied into C files and have their "extern" keyword removed in
# order to do a debug build with no optimisation 
list(APPEND EXTERN_INLINE_HEADER_LIST
	yuv_unpack
	yuv_to_rgb_convert
	yuv_to_rgb_convert_bt601
	yuv_to_rgb_convert_bt709
	rgb_pack
	rgb_unpack
	rgb_to_yuv_convert
	yuv_pack
)
function(GenerateExternInlines)
	foreach(CUR_FILE ${EXTERN_INLINE_HEADER_LIST})
		file(READ ${CUR_FILE}.h FILE_CONTENTS)
		string(REPLACE "EXTERN_INLINE " "inline " NEW_CONTENTS "${FILE_CONTENTS}")
		file(WRITE ${CUR_FILE}.c "${NEW_CONTENTS}")
	endforeach(CUR_FILE)
endfunction(GenerateExternInlines)


# Add compiler options
if(WIN32)
	include_directories(AFTER .)
else(WIN32)
	add_definitions(-msse2 -mssse3)
endif(WIN32)


# Check if we are building in debug mode
if(DEBUG)
	add_definitions(-DDEBUG)
	GenerateExternInlines()

	# Add extern inline definitions to source file list
	foreach(CUR_FILE ${EXTERN_INLINE_HEADER_LIST})
		list(APPEND PIXFC_SRC_LIST "${CUR_FILE}.c")
	endforeach(CUR_FILE)

	# Add compiler specific flags	
	if(APPLE)
		add_definitions(-gdwarf-2 -Wall -Winline)
	elseif(UNIX)
		add_definitions(-gdwarf-2 -Wall -Winline -Wno-trigraphs)
	endif(APPLE)
else(DEBUG)
	if(WIN32)
	else(WIN32)
		add_definitions(-O3 -Wno-trigraphs)

		# On Mac OS, memcpy is re-defined as a macro which introduces an "if" statement
		# and kills performance of RGB24 slower-repacking routines. To disable this
		# re-definition, we need to define the following:
		if(APPLE)
			add_definitions(-D_FORTIFY_SOURCE=0)
		endif(APPLE)

	endif(WIN32)
endif(DEBUG)

#
# LIBPIXFC-SSE TARGET
#
# Create groups for XCode and VS
source_group("source files" FILES ${PIXFC_SRC_LIST})
source_group("header files" FILES ${PIXFC_HDR_LIST})
# Create static library
add_library(pixfc-sse STATIC ${PIXFC_SRC_LIST} ${PIXFC_HDR_LIST})


#
# UNIT TESTING TARGET
#
# Create groups for XCode and VS
source_group("source files" FILES unit-testing.c ${TEST_UTIL_SRC_LIST})
source_group("header files" FILES ${PIXFC_HDR_LIST})
# Create unit testing app
add_executable(unit-testing unit-testing.c ${TEST_UTIL_SRC_LIST} ${PIXFC_HDR_LIST})
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
source_group("header files" FILES ${PIXFC_HDR_LIST})
# Create unit testing app
add_executable(convert-to-file convert-to-file.c ${TEST_UTIL_SRC_LIST} ${PIXFC_HDR_LIST})
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
source_group("header files" FILES ${PIXFC_HDR_LIST})
# Create visual check app
add_executable(visual-check visual-check.c ${TEST_UTIL_SRC_LIST} ${PIXFC_HDR_LIST})
if(WIN32 OR APPLE)
	target_link_libraries(visual-check pixfc-sse)
else(WIN32 OR APPLE)
	target_link_libraries(visual-check pixfc-sse rt)
endif(WIN32 OR APPLE)

#
# CLEAN TARGET
#
foreach(CUR_FILE ${EXTERN_INLINE_HEADER_LIST})
	list(APPEND EXTERN_INLINE_SRC_LIST "${CUR_FILE}.c")
endforeach(CUR_FILE)
add_custom_target(cleanup COMMAND rm -Rf CMakeFiles/ CMakeCache.txt cmake_install.cmake Makefile PixFC.xcodeproj/ PixFC.build/ CMakeScripts/ Debug/ build/ svn-* *~ libpixfc.a *.o *.ppm unit-testing convert-to-file visual-check ${EXTERN_INLINE_SRC_LIST})

