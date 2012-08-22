/*
 * unit-testing.c
 *
 * Copyright (C) 2011 PixFC Team (pixelfc@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public  License as published by the
 * Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "conversion_blocks.h"
#include "conversion_routines_from_v210.h"
#include "conversion_routines_from_argb.h"
#include "conversion_routines_from_bgr24.h"
#include "conversion_routines_from_yuyv.h"
#include "conversion_routines_from_yuv422p.h"
#include "conversion_routines_from_yuv420p.h"
#include "pixfc-sse.h"
#include "pixfmt_descriptions.h"
#include "platform_util.h"
#include "test-utils.h"


static uint32_t			check_formats_enum() {
	uint32_t		index;
	uint32_t		result = 0;

	// Enumerate the entire PixFcSSE pixel formats
	for(index = 0; index<pixfmt_descriptions_count; index++) {

		// Each pixel format description has a member indicating which
		// PixFcPixelFormat it describes. Also, the pixel format
		// descriptions array must be sorted according to the
		// PixFcPixelFormat enum. Here we enforce this rule.
		if (pixfmt_descriptions[index].pixFcFormat != index) {
			pixfc_log("Element at index %d in pixel format description does not"
					" match the expected PixFcPixelFormat enum entry (it "
					"matches entry %d)\n", index, pixfmt_descriptions[index].pixFcFormat);
			result = -1;
		}
	}


	// Check that both arrays have the same number of elements
	if (index != PixFcFormatCount) {
		pixfc_log("The number of pixel format descriptions (%d does not"
				" match the number of PixFcPixelFormat entries (%d)).\n",
				index, PixFcFormatCount);
		result = -1;
	}

	return result;
}

static int			check_unaligned_conversions() {
	struct PixFcSSE *				pixfc;
	void *							input[2] = { NULL };	// 1 aligned & 1 unaligned input buffer
	void *							output[2] = { NULL };	// 1 aligned & 1 unaligned output buffer
	uint32_t						w = 96,	h = 2, index = 0;

	// Loop over all conversion blocks
	for(index = 0; index < conversion_blocks_count; index++) {
		if (create_pixfc_for_conversion_block(index, &pixfc, w, h) != 0) {
			pixfc_log("Unable to test conversion block '%s'\n", conversion_blocks[index].name);
			continue;
		}

		pixfc_log("%-80s\n", conversion_blocks[index].name);

		// Allocate the input & output buffers
		if (allocate_aligned_buffer(pixfc->source_fmt, w, h, &input[0]) != 0) {
			pixfc_log("Error allocating aligned input buffer\n");
			return -1;
		}
		if (allocate_unaligned_buffer(pixfc->source_fmt, w, h, &input[1]) != 0) {
			pixfc_log("Error allocating unaligned input buffer\n");
			return -1;
		}
		if (allocate_aligned_buffer(pixfc->dest_fmt, w, h, &output[0]) != 0) {
			pixfc_log("Error allocating aligned output buffer\n");
			return -1;
		}
		if (allocate_unaligned_buffer(pixfc->dest_fmt, w, h, &output[1]) != 0) {
			pixfc_log("Error allocating unaligned output buffer\n");
			return -1;
		}

		// Fill input buffers
		fill_image(pixfc->source_fmt, IMG_SIZE(pixfc->source_fmt, w, h), input[0]);
		fill_image(pixfc->source_fmt, IMG_SIZE(pixfc->source_fmt, w, h), input[1]);

		// Do conversion with aligned input & output buffers
		pixfc->convert(pixfc, input[0], output[0]);
		pixfc_log(".\n");

		// Do conversion with aligned input & unaligned output buffers
		pixfc->convert(pixfc, input[0], output[1]);
		pixfc_log(".\n");

		// Do conversion with unaligned input & aligned output buffers
		pixfc->convert(pixfc, input[1], output[0]);
		pixfc_log(".\n");

		// Do conversion with unaligned input & output buffers
		pixfc->convert(pixfc, input[1], output[1]);
		pixfc_log(". OK !\n");

		// Free resources
		destroy_pixfc(pixfc);
		ALIGN_FREE(input[0]);
		free(input[1]);
		ALIGN_FREE(output[0]);
		free(output[1]);
	}

	return 0;
}

static uint32_t		check_conversion_enumeration() {
	uint32_t 			index;
	struct PixFcSSE		*pixfc = NULL;
	uint32_t			w = 96, h = 2;
	char				*csv_name = NULL;

	for(index = 0; index < conversion_blocks_count; index++) {
		pixfc_log("Checking '%s' ... \n", conversion_blocks[index].name);
		
		if (create_pixfc_for_conversion_block(index, &pixfc, w, h) != 0) {
			pixfc_log("Unable to test conversion block\n");
			continue;
		}

		if (pixfc->convert != conversion_blocks[index].convert_fn) {
			pixfc_log("Wrong conversion function returned for index %u\n", index);
			return -1;
		}

		if (pixfc->source_fmt!= conversion_blocks[index].source_fmt) {
			pixfc_log("Wrong source format returned for index %u\n", index);
			return -1;
		}

		if (pixfc->dest_fmt!= conversion_blocks[index].dest_fmt) {
			pixfc_log("Wrong destination format returned for index %u\n", index);
			return -1;
		}
		
		if (make_conv_block_name_csv_friendly(index, &csv_name) != 0) {
			pixfc_log("Error creating CSV friendly name for this conversion block\n");
			return -1;
		}
		pixfc_log(" OK !\n");
	}

	return 0;
}

static uint32_t 	do_flag_check(PixFcPixelFormat source_fmt, PixFcPixelFormat dest_fmt, PixFcFlag flag, PixFcFlag expected_flags, ConversionBlockFn expected_conv_fn) {
	struct PixFcSSE *	pixfc;
	uint32_t			result = -1;

	if (create_pixfc(&pixfc, source_fmt, dest_fmt, 96, 2, ROW_SIZE(source_fmt, 96), ROW_SIZE(dest_fmt, 96), flag) == 0) {
		if (pixfc->convert != expected_conv_fn)
			pixfc_log("Wrong conversion function\n");
		else if (expected_flags != pixfc->flags)
			pixfc_log("Wrong flags returned: %08x - expected %08x\n", pixfc->flags, expected_flags);
		else
			result = 0;

		destroy_pixfc(pixfc);
	} else
		pixfc_log("Error creating pixfc\n");

	return result;
}

// Run various checks on conversion routines' flags
static uint32_t	check_pixfc_flags() {
	uint32_t index;

	// TODO: Update here when new flags are added

	// Here, we check all conversion routines and make sure that if the
	// NONSSE_FLOAT_CONVERSION attribute is set, required_cpu_feature
	// is set to CPUID_FEATURE_NONE
	for(index = 0; index < conversion_blocks_count; index++){
		if (((conversion_blocks[index].attributes & NONSSE_FLOAT_CONVERSION) != 0) &&
				(conversion_blocks[index].required_cpu_features != CPUID_FEATURE_NONE)) {
			pixfc_log("NONSSE_FLOAT_CONVERSION attribute set but required_cpu_features != CPUID_FEATURE_NONE on conversion '%s'\n",
					conversion_blocks[index].name);
			return -1;
		}
	}


#define	CHECK_FLAGS(label, src_fmt, dst_fmt, flags_to_check, expected_result_flags, expected_conv_fn) \
	do { \
		pixfc_log("Checking " label " flags\n");\
		if (do_flag_check(src_fmt, dst_fmt, flags_to_check, expected_result_flags ,expected_conv_fn) != 0) {\
			pixfc_log(label " flags check failed\n");\
			return -1;\
		}\
		pixfc_log("OK !\n");\
	}while(0)

#define	CHECK_FLAGS_FAIL(label, src_fmt, dst_fmt, flags_to_check, expected_result_flags, expected_conv_fn) \
	do { \
		pixfc_log("Checking " label " flags\n");\
		if (do_flag_check(src_fmt, dst_fmt, flags_to_check, expected_result_flags ,expected_conv_fn) == 0) {\
			pixfc_log(label " flags check failed\n");\
			return -1;\
		}\
		pixfc_log("OK !\n");\
	}while(0)

	//
	// Checks that passing PixFcFlags_* results in the right conversion block
	// being chosen and the right flags being set in struct pixfc->flags

	// Default flag
	CHECK_FLAGS("default", PixFcV210, PixFcARGB,
			PixFcFlag_Default, PixFcFlag_SSE2_SSSE3_SSE41Only,
			upsample_n_convert_v210_to_argb_sse2_ssse3_sse41);



	//
	// NoSSE
	CHECK_FLAGS("NoSSE", PixFcV210, PixFcARGB,
			PixFcFlag_NoSSE, PixFcFlag_NoSSE,
			upsample_n_convert_v210_to_any_rgb_nonsse);


	// NoSSE | NNB
	CHECK_FLAGS("NoSSE | NNB ", PixFcV210, PixFcARGB,
			PixFcFlag_NoSSE  | PixFcFlag_NNbResamplingOnly,
			PixFcFlag_NoSSE  | PixFcFlag_NNbResamplingOnly,
			convert_v210_to_any_rgb_nonsse);


	// NoSSE | BT601
	CHECK_FLAGS("NoSSE | BT601", PixFcYUYV, PixFcARGB,
			PixFcFlag_NoSSE  | PixFcFlag_BT601Conversion,
			PixFcFlag_NoSSE  | PixFcFlag_BT601Conversion,
			upsample_n_convert_yuv422i_to_any_rgb_bt601_nonsse);

	// NoSSE | BT709
	CHECK_FLAGS("NoSSE | BT709", PixFcUYVY, PixFcARGB,
			PixFcFlag_NoSSE  | PixFcFlag_BT709Conversion,
			PixFcFlag_NoSSE  | PixFcFlag_BT709Conversion,
			upsample_n_convert_yuv422i_to_any_rgb_bt709_nonsse);

	// NoSSE | BT601 | NNB
	CHECK_FLAGS("NoSSE | BT601 | NNB", PixFcYUV420P, PixFcARGB,
			PixFcFlag_NoSSE  | PixFcFlag_BT601Conversion | PixFcFlag_NNbResamplingOnly,
			PixFcFlag_NoSSE  | PixFcFlag_BT601Conversion | PixFcFlag_NNbResamplingOnly,
			convert_yuv420p_to_any_rgb_bt601_nonsse);

	// NoSSE | BT709 | NNB
	CHECK_FLAGS("NoSSE | BT709 | NNB", PixFcYUYV, PixFcBGRA,
			PixFcFlag_NoSSE  | PixFcFlag_BT709Conversion | PixFcFlag_NNbResamplingOnly,
			PixFcFlag_NoSSE  | PixFcFlag_BT709Conversion | PixFcFlag_NNbResamplingOnly,
			convert_yuv422i_to_any_rgb_bt709_nonsse);




	//
	// NNB flag
	CHECK_FLAGS("NNB", PixFcV210, PixFcARGB,
			PixFcFlag_NNbResamplingOnly,
			PixFcFlag_NNbResamplingOnly| PixFcFlag_SSE2_SSSE3_SSE41Only,
			convert_v210_to_argb_sse2_ssse3_sse41);

	// BT601
	CHECK_FLAGS("BT601", PixFcV210, PixFcARGB,
			PixFcFlag_BT601Conversion,
			PixFcFlag_BT601Conversion | PixFcFlag_SSE2_SSSE3_SSE41Only,
			upsample_n_convert_v210_to_argb_bt601_sse2_ssse3_sse41);

	// BT709
	CHECK_FLAGS("BT709", PixFcYUYV, PixFcR210,
			PixFcFlag_BT709Conversion,
			PixFcFlag_BT709Conversion | PixFcFlag_SSE2_SSSE3Only,
			upsample_n_convert_yuyv_to_r210_bt709_sse2_ssse3);


	// BT601 | NNB
	CHECK_FLAGS("BT601 | NNB", PixFcV210, PixFcARGB,
			PixFcFlag_BT601Conversion | PixFcFlag_NNbResamplingOnly,
			PixFcFlag_BT601Conversion | PixFcFlag_NNbResamplingOnly | PixFcFlag_SSE2_SSSE3_SSE41Only,
			convert_v210_to_argb_bt601_sse2_ssse3_sse41);

	// BT709 | NNB
	CHECK_FLAGS("BT709 | NNB", PixFcYUYV, PixFcR210,
			PixFcFlag_BT709Conversion | PixFcFlag_NNbResamplingOnly,
			PixFcFlag_BT709Conversion | PixFcFlag_NNbResamplingOnly | PixFcFlag_SSE2_SSSE3Only,
			convert_yuyv_to_r210_bt709_sse2_ssse3);




	//
	// NoSSEFloat flag
	CHECK_FLAGS("NoSSEFloat", PixFcV210, PixFcARGB,
			PixFcFlag_NoSSEFloat, PixFcFlag_NoSSEFloat,
			upsample_n_convert_v210_to_any_rgb_nonsse_float);

	// NoSSEFloat | NNB flag
	CHECK_FLAGS("NoSSEFloat | NNB", PixFcV210, PixFcARGB,
			PixFcFlag_NoSSEFloat | PixFcFlag_NNbResamplingOnly,
			PixFcFlag_NoSSEFloat | PixFcFlag_NNbResamplingOnly,
			convert_v210_to_any_rgb_nonsse_float);

	// NoSSEFloat | BT601 flag
	CHECK_FLAGS("NoSSEFloat | BT601", PixFcYUV422P, PixFcRGB24,
			PixFcFlag_NoSSEFloat | PixFcFlag_BT601Conversion,
			PixFcFlag_NoSSEFloat | PixFcFlag_BT601Conversion,
			upsample_n_convert_yuv422p_to_any_rgb_bt601_nonsse_float);

	// NoSSEFloat | BT709 flag
	CHECK_FLAGS("NoSSEFloat | BT709", PixFcUYVY, PixFcBGR24,
			PixFcFlag_NoSSEFloat | PixFcFlag_BT709Conversion,
			PixFcFlag_NoSSEFloat | PixFcFlag_BT709Conversion,
			upsample_n_convert_yuv422i_to_any_rgb_bt709_nonsse_float);

	// NoSSEFloat | BT601 | NNB  flag
	CHECK_FLAGS("NoSSEFloat | BT601 | NNB", PixFcYUV422P, PixFcRGB24,
			PixFcFlag_NoSSEFloat | PixFcFlag_BT601Conversion | PixFcFlag_NNbResamplingOnly,
			PixFcFlag_NoSSEFloat | PixFcFlag_BT601Conversion | PixFcFlag_NNbResamplingOnly,
			convert_yuv422p_to_any_rgb_bt601_nonsse_float);

	// NoSSEFloat | BT709 | NNB  flag
	CHECK_FLAGS("NoSSEFloat | BT709 | NNB", PixFcUYVY, PixFcBGR24,
			PixFcFlag_NoSSEFloat | PixFcFlag_BT709Conversion | PixFcFlag_NNbResamplingOnly,
			PixFcFlag_NoSSEFloat | PixFcFlag_BT709Conversion | PixFcFlag_NNbResamplingOnly,
			convert_yuv422i_to_any_rgb_bt709_nonsse_float);


	// NoSSEFloat flag with repacking routines
	CHECK_FLAGS("NoSSEFloat w/ repacking fn", PixFcYUYV, PixFcV210,
			PixFcFlag_NoSSEFloat, PixFcFlag_NoSSEFloat,
			convert_yuv422i_to_v210_nonsse);




	//
	// NNB - This one is expected to fail
	CHECK_FLAGS_FAIL("NNB", PixFcV210, PixFcYUYV,
			PixFcFlag_NNbResamplingOnly,
			PixFcFlag_SSE2_SSSE3_SSE41Only | PixFcFlag_NNbResamplingOnly,
			convert_v210_to_any_rgb_nonsse);




	//
	// SSE2Only
	CHECK_FLAGS("SSE2Only", PixFcARGB, PixFcYUYV,
			PixFcFlag_SSE2Only, PixFcFlag_SSE2Only,
			downsample_n_convert_argb_to_yuyv_sse2);

	// SSE2Only | NNB flag
	CHECK_FLAGS("SSE2Only | NNB ", PixFcARGB, PixFcYUYV,
			PixFcFlag_SSE2Only | PixFcFlag_NNbResamplingOnly,
			PixFcFlag_SSE2Only | PixFcFlag_NNbResamplingOnly,
			convert_argb_to_yuyv_sse2);
	
	// SSE2Only | BT601
	CHECK_FLAGS("SSE2Only | BT601", PixFcARGB, PixFcYUYV,
			PixFcFlag_SSE2Only | PixFcFlag_BT601Conversion,
			PixFcFlag_SSE2Only | PixFcFlag_BT601Conversion,
			downsample_n_convert_argb_to_yuyv_bt601_sse2);

	// SSE2Only | B709
	CHECK_FLAGS("SSE2Only | BT709", PixFcARGB, PixFcYUYV,
			PixFcFlag_SSE2Only | PixFcFlag_BT709Conversion,
			PixFcFlag_SSE2Only | PixFcFlag_BT709Conversion,
			downsample_n_convert_argb_to_yuyv_bt709_sse2);

	// SSE2Only | BT601 | NNB
	CHECK_FLAGS("SSE2Only | BT601 | NNB", PixFcRGB24, PixFcUYVY,
			PixFcFlag_SSE2Only | PixFcFlag_BT601Conversion | PixFcFlag_NNbResamplingOnly,
			PixFcFlag_SSE2Only | PixFcFlag_BT601Conversion | PixFcFlag_NNbResamplingOnly,
			convert_bgr24_to_uyvy_bt601_sse2);

	// SSE2Only | B709 | NNB
	CHECK_FLAGS("SSE2Only | BT709 | NNB", PixFcBGR24, PixFcYUV422P,
			PixFcFlag_SSE2Only | PixFcFlag_BT709Conversion | PixFcFlag_NNbResamplingOnly,
			PixFcFlag_SSE2Only | PixFcFlag_BT709Conversion | PixFcFlag_NNbResamplingOnly,
			convert_bgr24_to_yuv422p_bt709_sse2);



	//
	// SSE2_SSSE3Only flag
	CHECK_FLAGS("SSE2_SSSE3Only", PixFcV210, PixFcARGB,
			PixFcFlag_SSE2_SSSE3Only, PixFcFlag_SSE2_SSSE3Only,
			upsample_n_convert_v210_to_argb_sse2_ssse3);

	// SSE2_SSSE3_SSE41Only flag
	CHECK_FLAGS("SSE2_SSSE3_SSE41Only", PixFcV210, PixFcARGB,
			PixFcFlag_SSE2_SSSE3_SSE41Only, PixFcFlag_SSE2_SSSE3_SSE41Only,
			upsample_n_convert_v210_to_argb_sse2_ssse3_sse41);


	return 0;
}

static uint32_t do_conversion(uint32_t index, uint32_t w, uint32_t h) {
	struct PixFcSSE *	pixfc;
	void *				input = NULL;	// 1 aligned & 1 unaligned input buffer
	void *				output = NULL;	// 1 aligned & 1 unaligned output buffer

	if (create_pixfc_for_conversion_block(index, &pixfc, w, h) != 0) {
		pixfc_log("Unable to test conversion block '%s'\n", conversion_blocks[index].name);
		return -1;
	}

	pixfc_log("%-80s%dx%d\n", conversion_blocks[index].name, w, h);

	// Allocate the input & output buffers
	if (allocate_aligned_buffer(pixfc->source_fmt, w, h, &input) != 0) {
		pixfc_log("Error allocating input buffer\n");
		return -1;
	}
	if (allocate_aligned_buffer(pixfc->dest_fmt, w, h, &output) != 0) {
		pixfc_log("Error allocating output buffer\n");
		ALIGN_FREE(input);
		return -1;
	}

	// Fill input buffers
	fill_image(pixfc->source_fmt, IMG_SIZE(pixfc->source_fmt, w, h), input);

	// Do conversion with aligned input & output buffers
	pixfc->convert(pixfc, input, output);

	// Free resources
	destroy_pixfc(pixfc);
	ALIGN_FREE(input);
	ALIGN_FREE(output);

	return 0;
}

static uint32_t check_conversion_buffer_sizes() {
	static const uint32_t	w_16_1[] = {16, 16*2, 16*3, 16*4, 16*5, 16*6, 16, 16*2, 16*3, 16*4, 16*5, 16*6, 720, 720, 960, 960+16, 960+32, 960+48, 1280, 1920};
	static const uint32_t	h_16_1[] = {1,  1,    1,    1,    1,    1,    2,  2,    2,    2,    2,    2,    480, 576, 720,  720,    720,    720,   1024, 1080};
	static const uint32_t 	size_count_16_1 = sizeof(w_16_1)/sizeof(w_16_1[0]);
	//
	static const uint32_t	w_32_1[] = {32, 32*2, 32*3, 32*4, 32*5, 32*6, 32, 32*2, 32*3, 32*4, 32*5, 32*6, 640, 960, 1280, 1920};
	static const uint32_t	h_32_1[] = {1,  1,    1,    1,    1,    1,    2,  2,    2,    2,    2,    2,    480, 720, 1024, 1080};
	static const uint32_t 	size_count_32_1 = sizeof(w_32_1)/sizeof(w_32_1[0]);
	//
	static const uint32_t	w_32_2[] = {32, 32*2, 32*3, 32*4, 32*5, 32*6, 736, 736, 960, 1280, 1920};
	static const uint32_t	h_32_2[] = {2,  2,    2,    2,    2,    2,    480, 576, 720, 1024, 1080};
	static const uint32_t 	size_count_32_2 = sizeof(w_32_2)/sizeof(w_32_2[0]);
	uint32_t				size_index = 0;

	//
	const uint32_t*		width_array = NULL;
	const uint32_t*		height_array = NULL;
	uint32_t			array_size = 0;
	uint32_t			conv_index = 0;

	// Loop over all conversion blocks
	for(conv_index = 0; conv_index < conversion_blocks_count; conv_index++) {
		if ((conversion_blocks[conv_index].width_multiple == 16) && (conversion_blocks[conv_index].height_multiple == 1)) {
			width_array = w_16_1;
			height_array = h_16_1;
			array_size = size_count_16_1;
		} else if ((conversion_blocks[conv_index].width_multiple == 32) && (conversion_blocks[conv_index].height_multiple == 1)) {
			width_array = w_32_1;
			height_array = h_32_1;
			array_size = size_count_32_1;
		} else if ((conversion_blocks[conv_index].width_multiple == 32) && (conversion_blocks[conv_index].height_multiple == 2)) {
			width_array = w_32_2;
			height_array = h_32_2;
			array_size = size_count_32_2;
		} else if ((conversion_blocks[conv_index].width_multiple == 2) && (conversion_blocks[conv_index].height_multiple == 2)) {
			// Non-sse YUV420p conversions
			width_array = w_32_2;
			height_array = h_32_2;
			array_size = size_count_32_2;
		} else if ((conversion_blocks[conv_index].width_multiple == 2) && (conversion_blocks[conv_index].height_multiple == 1)) {
			// Some non-sse conversions
			width_array = w_32_1;
			height_array = h_32_1;
			array_size = size_count_32_1;
		} else if ((conversion_blocks[conv_index].width_multiple == 1) && (conversion_blocks[conv_index].height_multiple == 2)) {
			// Some non-sse conversions
			width_array = w_32_2;
			height_array = h_32_2;
			array_size = size_count_32_2;
		} else if ((conversion_blocks[conv_index].width_multiple == 1) && (conversion_blocks[conv_index].height_multiple == 1)) {
			// All other non-sse conversions
			width_array = w_32_1;
			height_array = h_32_1;
			array_size = size_count_32_1;
		} else {
			pixfc_log("Unhandled width / height multiple: %u - %u for conversion %-80s\n",
					conversion_blocks[conv_index].width_multiple,
					conversion_blocks[conv_index].height_multiple,
					conversion_blocks[conv_index].name);
			return -1;
		}

		// try each image size
		for(size_index = 0; size_index < array_size; size_index ++) {
			if (do_conversion(conv_index, width_array[size_index], height_array[size_index]) != 0) {
				pixfc_log("Error testing conversion '%-80s'\n", conversion_blocks[conv_index].name);
				return -1;
			}
		}
	}

	return 0;
}

/*
 * Here we run a few tests to make sure things are sound internally
 */
int 				main(int argc, char **argv) {

	// SSE2 / SSSE3 and SSE41 are required to run this app.
	if (does_cpu_support(CPUID_FEATURE_SSE41 | CPUID_FEATURE_SSSE3 | CPUID_FEATURE_SSE2) != 0) {
		pixfc_log("CPU does not support required SSE features - exiting\n");
		return 1;
	}

	pixfc_log("\n");
	pixfc_log("\t\tU N I T   T E S T I N G\n");

	pixfc_log("\n");
	pixfc_log("Running internal checks ...\n");
	if (check_conversion_enumeration() != 0) {
		pixfc_log("FAILED\n");
		return -1;
	}
	pixfc_log("PASSED\n");


	pixfc_log("\n");
	pixfc_log("\n");
	pixfc_log("Running PixFcFlags checks ...\n");
	if (check_pixfc_flags()!= 0) {
		pixfc_log("FAILED\n");
		return -1;
	} else
		pixfc_log("PASSED\n");


	pixfc_log("\n");
	pixfc_log("\n");
	pixfc_log("Checking PixFcPixelFormat enum and description arrays...\n");
	if (check_formats_enum() != 0) {
		pixfc_log("FAILED\n");
		return -1;
	}
	pixfc_log("PASSED\n");

	pixfc_log("\n");
	pixfc_log("\n");
	pixfc_log("Testing conversion from aligned / unaligned buffers...\n");
	if (check_unaligned_conversions() != 0) {
		pixfc_log("FAILED\n");
		return -1;
	}
	pixfc_log("PASSED\n");

	pixfc_log("\n");
	pixfc_log("\n");
	pixfc_log("Testing conversions with multiple buffer sizes\n");
	if (check_conversion_buffer_sizes() != 0) {
		pixfc_log("FAILED\n");
		return -1;
	}
	pixfc_log("PASSED\n");
	
	return 0;
}




