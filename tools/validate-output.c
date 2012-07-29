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

static	PixFcPixelFormat		src_fmt = PixFcFormatCount;
static	PixFcPixelFormat		dst_fmt = PixFcFormatCount;
static  PixFcFlag				flags = PixFcFlag_Default;
//
static	PixFcPixelFormat		scalar_src_fmt = PixFcFormatCount;
static	PixFcPixelFormat		scalar_dst_fmt = PixFcFormatCount;
static  PixFcFlag				scalar_flags = PixFcFlag_Default;
//
static	uint8_t					max_diff = 0;
static const InputFile*			in_file = NULL;
// In / out buffers
static	__m128i	*				in = NULL;
static	__m128i	*				out = NULL;	// buffer holding SSE converted data
static	__m128i	*				out_scalar = NULL;// buffer holding scalar converted data for comparison



//
// Use the conversion block at the provided index to convert image of given width and height in in buffer to out buffer
static int		do_image_conversion(uint32_t index, void* in, void *out, uint32_t w, uint32_t h) {
	struct PixFcSSE *	pixfc = NULL;
	
	// Create struct pixfc
	if (create_pixfc_for_conversion_block(index, &pixfc, w, h) != 0) {
		pixfc_log("Error create struct pixfc\n");
		return -1;
	}

    pixfc_log("Checking  %s\n", conversion_blocks[index].name); 
	
	// Perform conversion
	pixfc->convert(pixfc, in, out);
		
	destroy_pixfc(pixfc);
	
	return 0;
}

static int		do_scalar_image_conversion(PixFcPixelFormat src_fmt, PixFcPixelFormat dest_fmt,  void* in, void *out, uint32_t w, uint32_t h) {
	struct PixFcSSE *	pixfc = NULL;
    uint32_t            index;
    uint32_t            scalar_flags = ((flags & ~(PixFcFlag_SSE2Only | PixFcFlag_SSE2_SSSE3Only)) | PixFcFlag_NoSSE);

    index = find_conversion_block_index(src_fmt, dest_fmt, scalar_flags, w, h, ROW_SIZE(src_fmt, w));
    if (index == -1) {
        pixfc_log("Error finding scalar conversion block\n");
        return -1;
    }

	// Create struct pixfc
	if (create_pixfc_for_conversion_block(index, &pixfc, w, h) != 0) {
		pixfc_log("Error create struct pixfc for scalar conversion\n");
		return -1;
	}

    pixfc_log("Using scalar conversion '%s'\n", conversion_blocks[index].name); 

	// Perform conversion
	pixfc->convert(pixfc, in, out);

	destroy_pixfc(pixfc);

	return 0;
}

static int		compare_8bit_output_buffers(uint8_t* out_sse, uint8_t* out_scalar, PixFcPixelFormat fmt, uint32_t width, uint32_t height, uint8_t max_diff) {
	uint32_t i;
	uint32_t buffer_size = IMG_SIZE(fmt, width, height);
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

#define COMPARE_VALUES(component)\
	if(abs(scalar_val - sse_val) > max_diff) {\
		printf(component " @ Pixel %ux%u (offset: %llu) differ by %u : SSE: %hhu - Scalar: %hhu\n",\
		(pixel + 1), line,\
		(unsigned long long)((sse_ptr - out_sse) * 4),\
		abs(scalar_val - sse_val), sse_val, scalar_val);\
		return -1;\
	}
static int		compare_v210_output_buffers(uint32_t* out_sse, uint32_t* out_scalar, PixFcPixelFormat fmt, uint32_t width, uint32_t height, uint8_t max_diff) {
	uint32_t line = 0;
	uint32_t pixel = 0;
	uint32_t bytes_per_row = ROW_SIZE(fmt,  width);
	uint32_t* scalar_ptr = out_scalar;
	uint32_t* sse_ptr = out_sse;
	uint16_t scalar_val;
	uint16_t sse_val;
	while(line++ < height) {
		while(pixel < width){
			scalar_val = (*scalar_ptr & 0x3FF);
			sse_val = (*sse_ptr & 0x3ff);
			COMPARE_VALUES("U");

			scalar_val = ((*scalar_ptr >> 10) & 0x3FF);
			sse_val = ((*sse_ptr >> 10) & 0x3ff);
			COMPARE_VALUES("Y1");

			scalar_val = ((*scalar_ptr++ >> 20) & 0x3FF);
			sse_val = ((*sse_ptr++ >> 20) & 0x3ff);
			COMPARE_VALUES("V");

			scalar_val = (*scalar_ptr & 0x3FF);
			sse_val = (*sse_ptr & 0x3ff);
			COMPARE_VALUES("Y2");

			scalar_val = ((*scalar_ptr >> 10) & 0x3FF);
			sse_val = ((*sse_ptr >> 10) & 0x3ff);
			COMPARE_VALUES("U");

			scalar_val = ((*scalar_ptr++ >> 20) & 0x3FF);
			sse_val = ((*sse_ptr++ >> 20) & 0x3ff);
			COMPARE_VALUES("Y3");

			scalar_val = (*scalar_ptr & 0x3FF);
			sse_val = (*sse_ptr & 0x3ff);
			COMPARE_VALUES("V");

			scalar_val = ((*scalar_ptr >> 10) & 0x3FF);
			sse_val = ((*sse_ptr >> 10) & 0x3ff);
			COMPARE_VALUES("Y4");

			scalar_val = ((*scalar_ptr++ >> 20) & 0x3FF);
			sse_val = ((*sse_ptr++ >> 20) & 0x3ff);
			COMPARE_VALUES("U");

			scalar_val = (*scalar_ptr & 0x3FF);
			sse_val = (*sse_ptr & 0x3ff);
			COMPARE_VALUES("Y5");

			scalar_val = ((*scalar_ptr >> 10) & 0x3FF);
			sse_val = ((*sse_ptr >> 10) & 0x3ff);
			COMPARE_VALUES("V");

			scalar_val = ((*scalar_ptr++ >> 20) & 0x3FF);
			sse_val = ((*sse_ptr++ >> 20) & 0x3ff);
			COMPARE_VALUES("Y6");

			pixel += 6;
		}
		scalar_ptr = out_scalar + line * bytes_per_row / 4;
		sse_ptr = out_sse + line * bytes_per_row / 4;
		pixel = 0;
	}

	return 0;
}


static int		compare_output_buffers(void* out_sse, void* out_scalar, PixFcPixelFormat fmt, uint32_t width, uint32_t height, uint8_t max_diff) {
	if (fmt == PixFcV210)
		return compare_v210_output_buffers((uint32_t*)out_sse, (uint32_t*)out_scalar, fmt, width, height, max_diff);
	else
		return compare_8bit_output_buffers((uint8_t*)out_sse, (uint8_t*)out_scalar, fmt, width, height, max_diff);
}

static void 	print_usage(char *prog_name) {
	printf("Usage: %s threshold [<src_pix_fmt> <dest_pix_fmt> <flags>]\n", prog_name);
	printf("Compare SSE conversion with scalar conversion, and stop if a difference\n");
	printf("between two values is greater than the provided threshold.\n");
	printf("Where <src_pix_fmt> and <dest_pix_fmt> are one of:\n");
	print_known_pixel_formats();
	printf("and flag is a bitmask from the following values:\n");
	print_known_flags();
}

static void 		parse_args(int argc, char **argv) {
	// Check and parse args
	if ((argc != 2) && (argc != 5)) {
		print_usage(argv[0]);
		exit(1);
	}

	max_diff = atoi(argv[1]);

	if (argc == 5){
		src_fmt = find_matching_pixel_format(argv[2]);
		if (src_fmt == PixFcFormatCount) {
			printf("Unknown pixel format '%s'\n", argv[2]);
			exit(1);
		}

		dst_fmt = find_matching_pixel_format(argv[3]);
		if (dst_fmt == PixFcFormatCount) {
			printf("Unknown pixel format '%s'\n", argv[3]);
			exit(1);
		}

        printf("Using:\n - source pixel format '%s'\n", pixfmt_descriptions[src_fmt].name);
        printf(" - dest pixel format '%s'\n", pixfmt_descriptions[dst_fmt].name);
		flags = get_matching_flags(argv[4]);
		printf(" - flags: ");
		print_flags(flags);
	}
}

static int 		setup_conversion_buffers() {
	// If we have changed input format...
	if (! in_file || (in_file->format != src_fmt)){
		// Release previous image and load image in new format
		ALIGN_FREE(in);
		
		in_file = find_input_file_for_format(src_fmt);
		if (in_file == NULL) {
			pixfc_log("Error looking for input file for format '%s'\n",
				   pixfmt_descriptions[src_fmt].name);
			return -1;
		}
		
		if (in_file->filename != NULL) {
			char 	in_filename[128] = {0};
			strcat(in_filename, PATH_TO_TEST_IMG);
			strcat(in_filename, in_file->filename);

			// Load buffer from specified file
			if (get_buffer_from_file(src_fmt, in_file->width, in_file->height, in_filename, (void **)&in) < 0) {
				pixfc_log("Error getting buffer from input file '%s'\n", in_file->filename);
				return -1;
			}
		} else {
			// no input file, assume its an RGB format
			// try to generate the image from RGB buffer from GIMP

			// allocate buffer
			if (allocate_aligned_buffer(src_fmt, in_file->width, in_file->height, (void **) &in)){
				pixfc_log("Error allocating memory\n");
				return -1;
			}
			if (fill_argb_image_with_rgb_buffer(src_fmt, in_file->width, in_file->height, in) != 0)
				fill_image(src_fmt, IMG_SIZE(src_fmt, in_file->width, in_file->height), in);
		}
	}

    if ((scalar_src_fmt != src_fmt) || (scalar_dst_fmt != dst_fmt) || (scalar_flags != flags )) {
        // If there was a format change, release previous output buffers and create new ones
        ALIGN_FREE(out);
        ALIGN_FREE(out_scalar);

        if (allocate_aligned_buffer(dst_fmt, in_file->width, in_file->height, (void **)&out) != 0) {
            pixfc_log("Error allocating output buffer");
            return -1;
        }

        if (allocate_aligned_buffer(dst_fmt, in_file->width, in_file->height, (void **)&out_scalar) != 0) {
            pixfc_log("Error allocating scalar output buffer");
            return -1;
        }
    }

	return 0;
}
static int		check_conversion_block(uint32_t index) {
	printf("%-60s %dx%d\n", conversion_blocks[index].name, in_file->width, in_file->height);

	// Do SSE conversion
	if (do_image_conversion(index, in, out, in_file->width, in_file->height) != 0)
		return -1;

	// Do scalar conversion if required
    if ((scalar_src_fmt != src_fmt) || (scalar_dst_fmt != dst_fmt) || (scalar_flags != flags )) {
       if (do_scalar_image_conversion(src_fmt, dst_fmt, in, out_scalar, in_file->width, in_file->height) != 0)
            return -1;

        scalar_flags = flags;
        scalar_src_fmt = src_fmt;
        scalar_dst_fmt = dst_fmt;
    }

	// Compare the two output buffers
	if (compare_output_buffers(out, out_scalar, dst_fmt, in_file->width, in_file->height, max_diff) != 0) {
		printf("Dumping scalar and sse buffers for inspection.\n");
		write_raw_buffer_to_file(dst_fmt, in_file->width, in_file->height, "sse_buffer.raw", out);
		write_raw_buffer_to_file(dst_fmt, in_file->width, in_file->height, "scalar_buffer.raw", out_scalar);
		return 1;
	}

	return 0;
}

int 			main(int argc, char **argv) {
	uint32_t				index;

	// Parse args
	parse_args(argc, argv);

	// Check if we have to restrict to specified source & dest formats
	if (src_fmt != PixFcFormatCount) {
		if (setup_conversion_buffers() == 0) {
			int32_t index;

			index = find_conversion_block_index(src_fmt, dst_fmt, flags, in_file->width, in_file->height, ROW_SIZE(src_fmt, in_file->width));
			if (index != -1)
				check_conversion_block(index);
			else
				printf("Cant find conversion block to do this conversion\n");
		} else
			printf("Error setting up conversion buffers\n");
	} else {
		// Loop over all conversion blocks
		for(index = 0; index < conversion_blocks_count; index++) {

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

			src_fmt = conversion_blocks[index].source_fmt;
			dst_fmt = conversion_blocks[index].dest_fmt;
            flags = conversion_blocks[index].attributes;

			if (setup_conversion_buffers() != 0) {
				printf("Error setting up conversion buffers\n");
				break;
			}

			if (check_conversion_block(index) != 0)
				break;

			// Add a blank line if the next conversion block uses different
			// source or destinaton format
			if (((index + 1) < conversion_blocks_count)
				&& ((conversion_blocks[(index + 1)].source_fmt !=
					 conversion_blocks[index].source_fmt)
					|| (conversion_blocks[(index + 1)].dest_fmt !=
						conversion_blocks[index].dest_fmt)))
				printf("\n");
		}
	}
	
	ALIGN_FREE(in);
	ALIGN_FREE(out);			   
	ALIGN_FREE(out_scalar);

	
	return 0;
}


