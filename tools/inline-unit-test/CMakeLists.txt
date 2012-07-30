include_directories(AFTER ../../src )

if(FAKE_SSE4)
    add_definitions(-DFAKE_SSE41_BLENDV)
endif(FAKE_SSE4)

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
	../../src/common.c
)
# add inline source file to source file list
foreach(CUR_FILE ${INLINE_FILENAME})
        list(APPEND SOURCE_LIST "${CUR_FILE}.c")
endforeach(CUR_FILE)

# List of header files (build from INLINE_FILENAME list)
foreach(CUR_FILE ${INLINE_FILENAME})
        list(APPEND HEADER_LIST "../../src/${CUR_FILE}.h")
endforeach(CUR_FILE)



source_group("source files" FILES ${SOURCE_LIST})
source_group("header files" FILES ${HEADER_LIST})
# Create unit testing app
add_executable(inline-unit-test ${SOURCE_LIST} ${HEADER_LIST})

