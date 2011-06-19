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

# include dirs
include_directories(AFTER include include/common)
if(WIN32)
	include_directories(AFTER include/win)
endif(WIN32)


add_subdirectory(src)
add_subdirectory(tools)
