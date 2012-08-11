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
project (PixFC-SSE C)

include(cmake/DetectTarget.cmake)

#
# Set include dirs
include_directories(AFTER include include/common)
if(WIN32)
	include_directories(AFTER include/win)
endif(WIN32)

#
# Set platform-specific GCC flags
#
# No cross compiling support for Windows yet.
# So only if not on Windows, add target CPU 
# specific compiler options
if(NOT WIN32)
    if(PIXFC_TARGET_ARCH MATCHES "Intel")
        add_definitions(-msse2 -mssse3 -msse4.1)
        if(PIXFC_TARGET_ARCH MATCHES "x86$")
			set(CMAKE_LINK_LIBRARY_FLAGS "-m32")
			set(CMAKE_EXE_LINKER_FLAGS "-m32")
            add_definitions(-m32)
            message("Build setup for Intel 32-bit CPU")
        elseif(PIXFC_TARGET_ARCH MATCHES "x86_64$")
			set(CMAKE_LINK_LIBRARY_FLAGS "-m64")
			set(CMAKE_EXE_LINKER_FLAGS "-m64")
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

#
# Release / Debug specific flags
#
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
                add_definitions(-O3 -Wall -Winline -Wno-implicit-function-declaration)
        endif(NOT WIN32)
endif(DEBUG)


IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Release CACHE STRING "If not already specified, set the type of build to Release"
				      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(tools/inline-unit-test)
