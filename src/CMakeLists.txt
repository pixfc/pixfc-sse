
# PixFC source files
list(APPEND PIXFC_SRC_LIST
	pixfc-sse.c
	common.c
   	conversion_blocks.c
	pixfmt_descriptions.c
	conversion_routines_from_yuyv.c
	conversion_routines_from_yuyv_bt601.c
	conversion_routines_from_yuyv_bt709.c
	conversion_routines_from_yuv422p.c
	conversion_routines_from_yuv422p_bt601.c
	conversion_routines_from_yuv422p_bt709.c
	conversion_routines_from_yuv420p.c
	conversion_routines_from_uyvy.c
	conversion_routines_from_uyvy_bt601.c
	conversion_routines_from_uyvy_bt709.c
	conversion_routines_from_argb.c
	conversion_routines_from_argb_bt601.c
	conversion_routines_from_argb_bt709.c
	conversion_routines_from_bgra.c
	conversion_routines_from_bgra_bt601.c
	conversion_routines_from_bgra_bt709.c
	conversion_routines_from_rgb24.c
	conversion_routines_from_rgb24_bt601.c
	conversion_routines_from_rgb24_bt709.c
	conversion_routines_from_bgr24.c
	conversion_routines_from_bgr24_bt601.c
	conversion_routines_from_bgr24_bt709.c
)

# header files
file(GLOB PIXFC_HDR_LIST *.h)	



# List of header files containing extern inlines definitions that must
# be copied into C files and have their "extern" keyword removed in
# order to do a debug build with no optimisation 
list(APPEND EXTERN_INLINE_HEADER_LIST
	yuv_repack
	yuv_upsample
	yuv_to_rgb_convert
	rgb_downsample
	rgb_to_yuv_convert
)
# List of header files containing extern inlines definitions that must
# be copied into C files, have their "extern" keyword removed and have
# the GENERATE_UNALIGNED_INLINES macro defined in order to do a debug 
# build with no optimisation 
list(APPEND EXTERN_INLINE_ALIGN_HEADER_LIST
	yuv_unpack
	rgb_pack
	rgb_unpack
	yuv_pack
)


# Add target CPU specific compiler options
if(NOT WIN32)
	if(PIXFC_TARGET_PROCESSOR MATCHES "Intel")
		add_definitions(-msse2 -mssse3)
		if(PIXFC_TARGET_PROCESSOR MATCHES "x86$")
			add_definitions(-m32)
			message("32-bit build")
		elseif(PIXFC_TARGET_PROCESSOR MATCHES "x86_64$")
			add_definitions(-m64)
			message("64-bit build")
		else(PIXFC_TARGET_PROCESSOR MATCHES "x86$")
			message(FATAL_ERROR "Unknown Intel CPU" ${PIXFC_TARGET_PROCESSOR})
		endif(PIXFC_TARGET_PROCESSOR MATCHES "x86$")
	elseif(PIXFC_TARGET_PROCESSOR STREQUAL "ARM")
		if(NOT DEFINED PIXFC_ARM_TARGET_PROCESSOR)
			message(FATAL_ERROR "PIXFC_ARM_TARGET_PROCESSOR not defined - define in toolchain CMAKE file: " ${CMAKE_TOOLCHAIN_FILE})
		endif(NOT DEFINED PIXFC_ARM_TARGET_PROCESSOR)

		message(${PIXFC_ARM_TARGET_PROCESSOR} " build")
		add_definitions(-mtune=${PIXFC_ARM_TARGET_PROCESSOR})
	endif(PIXFC_TARGET_PROCESSOR MATCHES "Intel")
endif(NOT WIN32)

# Check if we are building in debug mode
if(DEBUG)
	include_directories(AFTER .)
	add_definitions(-DDEBUG)

	# Add compiler specific flags	
	if(APPLE)
		add_definitions(-ggdb3 -O0 -Wall -Winline -Wno-implicit-function-declaration )
	elseif(UNIX)
		add_definitions(-ggdb3 -O0  -Wall -Winline -Wno-trigraphs -Wno-implicit-function-declaration)
	elseif(WIN32)
#		add_definitions(-Wno-implicit-function-declaration)
	endif(APPLE)

	# For each header file containing extern inlines, copy it to the binary dir into a source file
	foreach(CUR_FILE ${EXTERN_INLINE_HEADER_LIST})
		add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${CUR_FILE}.c DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${CUR_FILE}.h COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${CUR_FILE}.h ${CMAKE_CURRENT_BINARY_DIR}/${CUR_FILE}.c)
		list(APPEND PIXFC_SRC_LIST "${CMAKE_CURRENT_BINARY_DIR}/${CUR_FILE}.c")
	endforeach(CUR_FILE)


	# For each header file containing extern inlines that define aligned & unaligned pack/unpack, 
	# copy it to the binary dir into 2 source files (aligned & unaligned) and define the 
	# GENERATE_UNALIGNED_INLINES macro appropriately on each generated source file.
	foreach(CUR_FILE ${EXTERN_INLINE_ALIGN_HEADER_LIST})
		add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${CUR_FILE}_aligned.c DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${CUR_FILE}.h COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${CUR_FILE}.h ${CMAKE_CURRENT_BINARY_DIR}/${CUR_FILE}_aligned.c)
		add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${CUR_FILE}_unaligned.c DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${CUR_FILE}.h COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${CUR_FILE}.h ${CMAKE_CURRENT_BINARY_DIR}/${CUR_FILE}_unaligned.c)
		
		set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${CUR_FILE}_aligned.c PROPERTIES COMPILE_DEFINITIONS GENERATE_UNALIGNED_INLINES=0)
		set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${CUR_FILE}_unaligned.c PROPERTIES COMPILE_DEFINITIONS GENERATE_UNALIGNED_INLINES=1)

		list(APPEND PIXFC_SRC_LIST "${CMAKE_CURRENT_BINARY_DIR}/${CUR_FILE}_aligned.c" "${CMAKE_CURRENT_BINARY_DIR}/${CUR_FILE}_unaligned.c")
	endforeach(CUR_FILE)
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


