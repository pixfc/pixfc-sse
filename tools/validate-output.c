/*
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

#include "common.h"
#include "conversion_blocks.h"
#include "pixfc-sse.h"
#include "pixfmt_descriptions.h"
#include "platform_util.h"
#include "rgb_image.h"
#include "test-utils.h"

#ifdef __INTEL_CPU__
#include <emmintrin.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_DIFF	2

//
// Use the conversion block at the provided index to convert image of given width and height in in buffer to out buffer
static int		do_image_conversion(uint32_t index, void* in, void *out, uint32_t w, uint32_t h) {
	struct PixFcSSE *	pixfc = NULL;
	
	// Create struct pixfc
	if (create_pixfc_for_conversion_block(index, &pixfc, w, h) != 0) {
		pixfc_log("Error create struct pixfc\n");
		return -1;
	}
	
	// Perform conversion
	pixfc->convert(pixfc, in, out);
		
	destroy_pixfc(pixfc);
	
	return 0;
}

static int		do_scalar_image_conversion(PixFcPixelFormat src_fmt, PixFcPixelFormat dest_fmt,  void* in, void *out, uint32_t w, uint32_t h) {
	struct PixFcSSE *	pixfc = NULL;

	// Create struct pixfc
	if (create_pixfc(&pixfc, src_fmt, dest_fmt, w, h, PixFcFlag_NoSSE) != 0) {
		pixfc_log("Error create struct pixfc for scalar conversion\n");
		return -1;
	}

	// Perform conversion
	pixfc->convert(pixfc, in, out);

	destroy_pixfc(pixfc);

	return 0;
}

static int		compare_output_buffers(uint8_t* out_sse, uint8_t* out_scalar, PixFcPixelFormat fmt, uint32_t width, uint32_t height, uint8_t max_diff) {
	uint32_t i;
	uint32_t buffer_size = IMG_SIZE(fmt, width, height);
	uint32_t bytes_per_row = width * pixfmt_descriptions[fmt].bytes_per_pix_num / pixfmt_descriptions[fmt].bytes_per_pix_denom;
	uint8_t* sse_start = out_sse;
	for(i = 0; i < buffer_size; i++) {
		if(abs(*out_scalar - *out_sse) > max_diff) {
			printf("Pixel %ux%u (offset: %llu) differ by %u : SSE: %hhu - Scalar: %hhu\n",
					(i * pixfmt_descriptions[fmt].bytes_per_pix_denom / pixfmt_descriptions[fmt].bytes_per_pix_num) % width + 1,
					(i * pixfmt_descriptions[fmt].bytes_per_pix_denom / pixfmt_descriptions[fmt].bytes_per_pix_num) / width + 1,
					(unsigned long long)(out_sse - sse_start),
					abs(*out_scalar - *out_sse), *out_sse, *out_scalar);
			return -1;
		}
		out_scalar++;
		out_sse++;
	}

	return 0;
}

int 		main(int argc, char **argv) {
	uint32_t				index;
	const InputFile*		in_file = NULL;
	PixFcPixelFormat		src_fmt = PixFcFormatCount;
	uint8_t					max_diff = MAX_DIFF;
	// In / out buffers
	__m128i	*				in = NULL;
	__m128i	*				out = NULL;	// buffer holding SSE converted data
	__m128i	*				out_scalar = NULL;// buffer holding scalar converted data for comparison
	
	if (argc < 2) {
		printf("Usage: %s threshold [ source pixel format]\n", argv[0]);
		return 0;
	}

	// Check if we should restrict to a single source format
	if (argc >= 2)
		max_diff = atoi(argv[1]);

	if (argc == 3) {
		src_fmt = find_matching_pixel_format(argv[1]);
		if (src_fmt != PixFcFormatCount)
			printf("Using source pixel format '%s'\n", pixfmt_descriptions[src_fmt].name);
		else
		{
			printf("Unknown pixel format '%s'\n", argv[1]);
			exit(1);
		}
	}

	// Loop over all conversion blocks
	for(index = 0; index < conversion_blocks_count; index++) {
		// Check if we have to restrict to the specified source format
		if ((src_fmt != PixFcFormatCount) && (src_fmt != conversion_blocks[index].source_fmt))
			continue;
		
		// Make sure we skipped the scalar conversion routines
		// (we dont want to compare them with themselves)
		if (conversion_blocks[index].required_cpu_features == CPUID_FEATURE_NONE)
			continue;

		// Make sure the CPU has the required features
		if (does_cpu_support(conversion_blocks[index].required_cpu_features) != 0) {
			printf("  (skipped %s - CPU missing required features)\n",
				   conversion_blocks[index].name);
			continue;
		}
		
		// If we have changed input format...
		if (! in_file || (in_file->format != conversion_blocks[index].source_fmt)){
			// Release previous image and load image in new format
			ALIGN_FREE(in);
			
			in_file = find_input_file_for_format(conversion_blocks[index].source_fmt);
			if (in_file == NULL) {
				pixfc_log("Error looking for input file for format '%s'\n",
					   pixfmt_descriptions[conversion_blocks[index].source_fmt].name);
				return 1;
			}
			
			if (in_file->filename != NULL) {
				char 	in_filename[128] = {0};
				strcat(in_filename, PATH_TO_TEST_IMG);
				strcat(in_filename, in_file->filename);

				// Load buffer from specified file
				if (get_buffer_from_file(conversion_blocks[index].source_fmt, in_file->width, in_file->height, in_filename, (void **)&in) < 0) {
					pixfc_log("Error getting buffer from input file '%s'\n", in_file->filename);
					return 1;
				}
			} else {
				// no input file, assume its an RGB format
				// try to generate the image from RGB buffer from GIMP

				// allocate buffer
				if (allocate_aligned_buffer(conversion_blocks[index].source_fmt, in_file->width, in_file->height, (void **) &in)){
					pixfc_log("Error allocating memory\n");
					return -1;
				}
				if (fill_argb_image_with_rgb_buffer(conversion_blocks[index].source_fmt, in_file->width, in_file->height, in) != 0) {
					pixfc_log("Error getting buffer from RGB GIMP image\n");
					return 1;
				}
			}
		}
		
		// Release previous output buffers and create new ones
		ALIGN_FREE(out);
		ALIGN_FREE(out_scalar);
		
		if (allocate_aligned_buffer(conversion_blocks[index].dest_fmt, in_file->width, in_file->height, (void **)&out) != 0) {
			pixfc_log("Error allocating output buffer");
			return 1;
		}

		if (allocate_aligned_buffer(conversion_blocks[index].dest_fmt, in_file->width, in_file->height, (void **)&out_scalar) != 0) {
			pixfc_log("Error allocating scalar output buffer");
			return 1;
		}

		printf("%-60s %dx%d\n", conversion_blocks[index].name, in_file->width, in_file->height);
		
		// Do SSE conversion
		if (do_image_conversion(index, in, out, in_file->width, in_file->height) != 0)
			return -1;
		
		// Do scalar conversion
		if (do_scalar_image_conversion(conversion_blocks[index].source_fmt, conversion_blocks[index].dest_fmt, in, out_scalar, in_file->width, in_file->height) != 0)
			return -1;

		// Compare the two output buffers
		if (compare_output_buffers((uint8_t*)out, (uint8_t*)out_scalar, conversion_blocks[index].dest_fmt, in_file->width, in_file->height, max_diff) != 0) {
			break;
		}
		
		// Add a blank line if the next conversion block uses different
		// source or destinaton format
		if (((index + 1) < conversion_blocks_count)
			&& ((conversion_blocks[(index + 1)].source_fmt !=
				 conversion_blocks[index].source_fmt)
				|| (conversion_blocks[(index + 1)].dest_fmt !=
					conversion_blocks[index].dest_fmt)))
			printf("\n");
	}
	
	ALIGN_FREE(in);
	ALIGN_FREE(out);			   
	ALIGN_FREE(out_scalar);

	
	return 0;
}


